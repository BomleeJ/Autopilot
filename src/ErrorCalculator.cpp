#include <algorithm>
#include "ErrorCalculator.h"
Error ErrorCalculator::calculateSpeedError(const AircraftState& aircraft_state, const AircraftGuidance& guidance) {
    Error err = (guidance.kinematics_targets.indicated_airspeed_knots.value() 
        - aircraft_state.kinematics.indicated_airspeed_knots 
        ) / guidance.kinematics_targets.indicated_airspeed_knots.value();

    return std::clamp(err, -1.0f, 1.0f);
}

Error ErrorCalculator::calculateVspeedError(const AircraftState& aircraft_state, const AircraftGuidance& guidance) {
    Error err = (guidance.kinematics_targets.vertical_speed_fpm.value() 
        - aircraft_state.kinematics.vertical_speed_fpm 
        ) / guidance.kinematics_targets.vertical_speed_fpm.value();

    return std::clamp(err, -1.0f, 1.0f);
}

Error ErrorCalculator::calculatePhiError(const AircraftState& aircraft_state, Degrees roll_deg_target) {
    Error err = (roll_deg_target 
        - aircraft_state.attitude.roll_deg 
        ) / roll_deg_target;

    return std::clamp(err, -1.0f, 1.0f);
}

Error ErrorCalculator::calculateHeadingError(const AircraftState& aircraft_state, const AircraftGuidance& guidance) {
    Error err = (guidance.attitude_targets.true_heading_deg.value() 
        - aircraft_state.attitude.true_heading 
        ) / guidance.attitude_targets.true_heading_deg.value(); 

    return std::clamp(err, -1.0f, 1.0f);
}