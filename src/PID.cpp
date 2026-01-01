#include "PID.h"
#include "XPLMUtilities.h"
#include <algorithm>
#include <string>

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
    XPLMDebugString("Running Integral: ");
    XPLMDebugString(std::to_string(RunningIntegral).c_str());
    XPLMDebugString("\n");

    XPLMDebugString("Derivative: ");
    XPLMDebugString(std::to_string(Derivative).c_str());
    XPLMDebugString("\n");
    output += DerivativeConstant * Derivative;

    // INSERT_YOUR_CODE
    float pct_p = ProportionalConstant * error;
    float pct_i = IntegralConstant * RunningIntegral;
    float pct_d = DerivativeConstant * Derivative;


    float total = pct_p + pct_i + pct_d + 0.4f; // match total output before clamping

    char debugStr[128];
    char pStr[32], iStr[32], dStr[32], totalStr[32];

    snprintf(pStr, sizeof(pStr), "P=%.2f%% ", pct_p * 100.0f);
    snprintf(iStr, sizeof(iStr), "I=%.2f%% ", pct_i * 100.0f);
    snprintf(dStr, sizeof(dStr), "D=%.2f%% ", pct_d * 100.0f);
    snprintf(totalStr, sizeof(totalStr), "Total=%.2f%%\n", total * 100.0f);

    strcpy(debugStr, "Throttle PID breakdown: ");
    strcat(debugStr, pStr);
    strcat(debugStr, iStr);
    strcat(debugStr, dStr);
    strcat(debugStr, totalStr);

    XPLMDebugString(debugStr);
    XPLMDebugString("\n");

    return std::clamp(output, 0.0f, 1.0f);
}

Error ThrottlePIDController::calculateError(Knots target_airspeed_knots, Knots current_airspeed_knots) {
    float denom = std::max(target_airspeed_knots, 45.0f); // Stall speed
    return (target_airspeed_knots - current_airspeed_knots) / denom;
}

float PitchPIDController::calculate(float error) {
    return 0.0f;
}

float HeadingPIDController::calculate(float error) {
    return 0.0f;
}