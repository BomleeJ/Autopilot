#pragma once
#include "Types.h"
class PID {
    private:
    float ProportionalConstant;
    float IntegralConstant;
    float DerivativeConstant;
    float RunningIntegral;
    float Derivative;
    Error previous_error;
    public:
    PID(float kp, float ki, float kd);
    float calculate(float error);
    
};