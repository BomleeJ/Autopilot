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
};

ThrottlePIDController::ThrottlePIDController(float kp, float ki, float kd): 
PIDController(kp, ki, kd) 
{   }

HeadingPIDController::HeadingPIDController(float kp, float ki, float kd): 
PIDController(kp, ki, kd) 
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

