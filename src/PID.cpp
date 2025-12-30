#include "PID.h"
#include "XPLMUtilities.h"
#include <algorithm>

PID::PID(float kp, float ki, float kd): 
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

float PID::calculate(Error error) {
    float output = 0.0f;

    output += ProportionalConstant * error;

    if (previous_error != 0.0f) {
        Derivative = (error - previous_error);
    }
    previous_error = error;

    output += IntegralConstant * RunningIntegral;

    output += DerivativeConstant * Derivative;

    RunningIntegral += error;

    return std::clamp(output, -1.0f, 1.0f);
}