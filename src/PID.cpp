#include "PID.h"
#include "XPLMUtilities.h"
#include <algorithm>
#include <string>
#include <cstdio>
#include <cstring>


PIDController::PIDController(float kp, float ki, float kd): 
    ProportionalConstant(kp), 
    IntegralConstant(ki), 
    DerivativeConstant(kd), 
    RunningIntegral(0.0f), 
    Derivative(0.0f), 
    previous_error(0.0f) 
{ 
    if (kp + ki + kd >= 1.0f) {
        XPLMDebugString("WARNING: PID constructor: kp + ki + kd is greater than 1.0f, this can cause instability\n");
    }
}

float PIDController::calculate(float error) {
    float output = 0.0f;
    
    // Proportional term
    output += ProportionalConstant * error;
    
    // Derivative term
    if (previous_error != 0.0f) {
        Derivative = (error - previous_error);
    }
    previous_error = error;
    output += DerivativeConstant * Derivative;
    
    // Integral term with windup prevention/saturation
    output += IntegralConstant * RunningIntegral;
    RunningIntegral += error;
    RunningIntegral = std::clamp(RunningIntegral, -2.0f, 2.0f);  // Prevent integral windup
    
    // Return normalized output [-1, 1]
    return std::clamp(output, -1.0f, 1.0f);
}

ThrottlePIDController::ThrottlePIDController(float kp, float ki, float kd): 
PIDController(kp, ki, kd) 
{   }

HeadingPIDController::HeadingPIDController(float kp, float ki, float kd): 
PIDController(kp, ki, kd) 
{   }

PitchPIDController::PitchPIDController(float kp_altitude, float ki_altitude, float kd_altitude,
                                       float kp_vspeed, float ki_vspeed, float kd_vspeed):
    PIDController(0.0f, 0.0f, 0.0f),  // Base class initialized with zeros since we use internal PIDs
    altitudePID(kp_altitude, ki_altitude, kd_altitude),
    vspeedPID(kp_vspeed, ki_vspeed, kd_vspeed),
    max_vspeed(900.0f)  // Default max vspeed for normalization (900 fpm for Cessna 172)
{   }

float ThrottlePIDController::calculate(float error) {
    float output = 0.6f;

    output += ProportionalConstant * error;
    
    if (previous_error != 0.0f) 
    {
        Derivative = (error - previous_error);
    }
    previous_error = error;

    output += IntegralConstant * RunningIntegral;

    RunningIntegral += error;
    RunningIntegral = std::clamp(RunningIntegral, -2.0f, 2.0f);
    output += DerivativeConstant * Derivative;

    return std::clamp(output, 0.0f, 1.0f);
}

Error ThrottlePIDController::calculateError(Knots target_airspeed_knots, Knots current_airspeed_knots) {
    float denom = std::max(target_airspeed_knots, 45.0f); // Stall speed
    return (target_airspeed_knots - current_airspeed_knots) / denom;
}

float PitchPIDController::calculate(float error) {
    return 0.0f;
}

Error PitchPIDController::calculateAltitudeError(Feet target_altitude, Feet current_altitude) {
    float raw_error = target_altitude - current_altitude;
    // Normalize by dividing by max altitude error (1000 ft) or target altitude, whichever is larger
    // REPLACES TO JUST BE divided 1000
    Error normalized_error = raw_error / 1000;
    
    char debugStr[256];
    snprintf(debugStr, sizeof(debugStr), "Altitude Error: Current=%.2f ft, Target=%.2f ft, Raw Error=%.2f ft, Normalized=%.4f\n", 
             current_altitude, target_altitude, raw_error, normalized_error);
    XPLMDebugString(debugStr);
    
    return std::clamp(normalized_error, -1.0f, 1.0f);
}

Error PitchPIDController::calculateVspeedError(FeetPerMinute target_vspeed, FeetPerMinute current_vspeed) {
    float raw_error = target_vspeed - current_vspeed;
    // Normalize by dividing by max vspeed
    Error normalized_error = raw_error / max_vspeed;
    
    char debugStr[256];
    snprintf(debugStr, sizeof(debugStr), "Vspeed Error: Current=%.2f fpm, Target=%.2f fpm, Raw Error=%.2f fpm, Normalized=%.4f\n", 
             current_vspeed, target_vspeed, raw_error, normalized_error);
    XPLMDebugString(debugStr);
    
    return std::clamp(normalized_error, -1.0f, 1.0f);
}

FeetPerMinute PitchPIDController::calculateVspeedTarget(Error altitude_error) {
    // Use altitude PID to get normalized output
    // Note: altitudePID.calculate() includes integral windup prevention (clamped to [-2.0, 2.0])
    float normalized_output = altitudePID.calculate(altitude_error);
    
    // Convert normalized output [-1, 1] to vspeed target
    // For climb (positive): map [0, 1] -> [0, 1000] fpm (max climb)
    // For descent (negative): map [-1, 0] -> [-800, 0] fpm (max descent)
    FeetPerMinute vspeed_target;
    if (normalized_output >= 0.0f) {
        // Climb: scale to [0, 1000] fpm
        vspeed_target = normalized_output * 800.0f;
    } else {
        // Descent: scale to [-800, 0] fpm
        vspeed_target = normalized_output * 800.0f;
    }
    
    // Clamp to reasonable limits: -800 fpm (descent) to 1000 fpm (climb)
    vspeed_target = std::clamp(vspeed_target, -800.0f, 800.0f);
    
    char debugStr[256];
    snprintf(debugStr, sizeof(debugStr), "Vspeed Target: Altitude Error=%.4f, Normalized Output=%.4f, Vspeed Target=%.2f fpm\n", 
             altitude_error, normalized_output, vspeed_target);
    XPLMDebugString(debugStr);
    
    return vspeed_target;
}

float PitchPIDController::getPitchCommand(AircraftState& aircraft_state, AircraftGuidance& guidance) {
    FeetPerMinute target_vspeed;
    FeetPerMinute current_vspeed = aircraft_state.kinematics.vertical_speed_fpm;
    
    // Determine target vspeed: either from guidance or calculated from altitude error
    if (guidance.position_targets.altitude_msl_ft.has_value()) {
        // Calculate vspeed target from altitude error
        Feet target_altitude = guidance.position_targets.altitude_msl_ft.value();
        Feet current_altitude = aircraft_state.position.altitude_msl_ft;
        Error altitude_error = calculateAltitudeError(target_altitude, current_altitude);
        target_vspeed = calculateVspeedTarget(altitude_error);
    } else if (guidance.kinematics_targets.vertical_speed_fpm.has_value()) {
        // Use direct vspeed target from guidance
        target_vspeed = guidance.kinematics_targets.vertical_speed_fpm.value();
        
        char debugStr[128];
        snprintf(debugStr, sizeof(debugStr), "Using direct vspeed target from guidance: %.2f fpm\n", target_vspeed);
        XPLMDebugString(debugStr);
    } else {
        // No altitude or vspeed target, return neutral pitch
        XPLMDebugString("WARNING: No altitude or vspeed target in guidance, returning neutral pitch\n");
        return 0.0f;
    }
    
    // Calculate vspeed error and use vspeed PID to get pitch command
    // Note: vspeedPID.calculate() includes integral windup prevention (clamped to [-2.0, 2.0])
    Error vspeed_error = calculateVspeedError(target_vspeed, current_vspeed);
    float normalized_pitch_output = vspeedPID.calculate(vspeed_error);
    
    // Convert normalized output [-1, 1] to pitch yoke command [-0.5, 0.5]
    // This matches the range used in HeadingPIDController
    float pitch_command = normalized_pitch_output * 0.5f;
    pitch_command = std::clamp(pitch_command, -0.5f, 0.5f);
    
    char debugStr[256];
    snprintf(debugStr, sizeof(debugStr), "Pitch Command: Vspeed Error=%.4f, Normalized Output=%.4f, Pitch Command=%.4f\n", 
             vspeed_error, normalized_pitch_output, pitch_command);
    XPLMDebugString(debugStr);
    XPLMDebugString("\n");
    
    return pitch_command;
}

Degrees HeadingPIDController::calculateHeadingError(Degrees current_heading, Degrees target_heading) {
    float error = target_heading - current_heading;
    
    // Normalize to [-180, 180]
    while (error > 180.0f) error -= 360.0f;
    while (error < -180.0f) error += 360.0f;
    
    char debugStr[128];
    snprintf(debugStr, sizeof(debugStr), "Heading Error: Current=%.2f deg, Target=%.2f deg, Error=%.2f deg\n", 
             current_heading, target_heading, error);
    XPLMDebugString(debugStr);
    XPLMDebugString("\n");
    
    return error;
}

float HeadingPIDController::calculatePhiTarget(Degrees current_hpath, Degrees target_hpath) {
    float error = calculateHeadingError(current_hpath, target_hpath);
    float maxBankAngle = 25.0f;
    float phi_target = std::clamp(error, -maxBankAngle, maxBankAngle);

    char debugStr[128];
    snprintf(debugStr, sizeof(debugStr), "Phi Target: Error=%.2f deg, Phi Target=%.2f deg\n", 
             error, phi_target);
    XPLMDebugString(debugStr);
    XPLMDebugString("\n");

    return phi_target;

}

float HeadingPIDController::calculatePhiError(Degrees phi_target, Degrees phi_current) {
    float error = (phi_target - phi_current);
    
    char debugStr[128];
    snprintf(debugStr, sizeof(debugStr), "Phi Error: Phi Target=%.2f deg, Phi Current=%.2f deg, Error=%.4f\n", 
             phi_target, phi_current, error);
    XPLMDebugString(debugStr);
    XPLMDebugString("\n");
    
    return error;
}

float HeadingPIDController::getYokeCommand(AircraftState& aircraft_state, AircraftGuidance& guidance) {
    Degrees current_hpath = aircraft_state.attitude.flight_path_heading_deg;
    Degrees target_hpath = guidance.attitude_targets.flight_path_heading_deg.value();
    Degrees phi_target = calculatePhiTarget(current_hpath, target_hpath);
    Degrees phi_current = aircraft_state.attitude.roll_deg;
    float phi_error = calculatePhiError(phi_target, phi_current);
    phi_error = phi_error * ProportionalConstant;
    return std::clamp(phi_error, -0.5f, 0.5f);
}

