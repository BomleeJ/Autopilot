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
    float calculate(float error);  // Basic PID calculation, returns normalized output [-1, 1]
    
};

class ThrottlePIDController : public PIDController {
    public:
    ThrottlePIDController(float kp, float ki, float kd);
    float calculate(float error);
    Error calculateError(Knots target_airspeed_knots, Knots current_airspeed_knots);
};

class PitchPIDController : public PIDController {
    private:
    PIDController altitudePID;
    PIDController vspeedPID;
    FeetPerMinute max_vspeed;
    
    Error calculateAltitudeError(Feet target_altitude, Feet current_altitude);
    Error calculateVspeedError(FeetPerMinute target_vspeed, FeetPerMinute current_vspeed);
    FeetPerMinute calculateVspeedTarget(Error altitude_error);
    
    public:
    PitchPIDController(float kp_altitude, float ki_altitude, float kd_altitude, 
                       float kp_vspeed, float ki_vspeed, float kd_vspeed);
    float calculate(float error);
    float getPitchCommand(AircraftState& aircraft_state, AircraftGuidance& guidance);
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