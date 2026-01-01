#pragma once
#include "Types.h"
#include "AircraftState.h"
#include "AircraftGuidance.h"
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
    
};

class ThrottlePIDController : public PIDController {
    public:
    ThrottlePIDController(float kp, float ki, float kd);
    float calculate(float error);
    Error calculateError(Knots target_airspeed_knots, Knots current_airspeed_knots);
};

class PitchPIDController : public PIDController {
    public:
    float calculate(float error);
};

class HeadingPIDController : public PIDController {
    private:
    Degrees calculateHeadingError(Degrees current_heading, Degrees target_heading);
    Degrees calculatePhiTarget(Degrees current_hpath, Degrees target_hpath);
    float calculatePhiError(Degrees phi_target, Degrees phi_current);
    public:
    HeadingPIDController(float kp, float ki, float kd);
    
    float getYokeCommand(AircraftState& aircraft_state, AircraftGuidance& guidance);
    
};