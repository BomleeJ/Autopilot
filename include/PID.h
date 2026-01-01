#pragma once
#include "Types.h"
class PIDController {
    protected:
    float ProportionalConstant;
    float IntegralConstant;
    float DerivativeConstant;
    float RunningIntegral;
    float Derivative;
    Error previous_error;
    public:
    PIDController(float kp, float ki, float kd);
    virtual float calculate(float error) = 0;
    
};

class ThrottlePIDController : public PIDController {
    public:
    ThrottlePIDController(float kp, float ki, float kd);
    float calculate(float error) override;
    Error calculateError(Knots target_airspeed_knots, Knots current_airspeed_knots);
};

class PitchPIDController : public PIDController {
    public:
    float calculate(float error) override;
};

class HeadingPIDController : public PIDController {
    public:
    float calculate(float error) override;
};