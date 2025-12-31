#include "FlightState.h"
#include "AircraftGuidance.h"
#include "AircraftState.h"
#include <optional>
#include "Types.h"
#include "Waypoint.h"

Knots cessna172_VRoateSpeed = 55;
Knots cessna172_CruiseSpeed = 110;
FeetPerMinute cessna172_InitialClimbRateFpm = 900;
FeetPerMinute cessna172_ClimbToAltitudeRateFpm = 700;
Feet cessna172_InitialClimbAltitudeFt = 800;
Feet cessna172_CruiseAltitudeFt = 8500;

FlightStateManager::FlightStateManager(): activated(false) {
    current_state = FlightState::TAKEOFF_ROLL;
}

AircraftGuidance FlightStateManager::getGuidance(const AircraftState& aircraft_state, NavigationManager& navigation_manager) 
{
    std::optional<Waypoint> current_waypoint = navigation_manager.getCurrentWaypoint(aircraft_state);
    if (!activated) {
        setGuidanceForTakeoffRoll(aircraft_state);
        activated = true;
    }
    else if (shouldTransitionToInitialClimb(aircraft_state)) {
        setGuidanceForInitialClimb(aircraft_state);
        current_state = FlightState::INITIAL_CLIMB;
    }
    else if (shouldTransitionToClimbToAltitude(aircraft_state)) {
        setGuidanceForClimbToAltitude(current_waypoint, aircraft_state);
        current_state = FlightState::CLIMB_TO_ALTITUDE;
    }
    else if (shouldTransitionToCruise(aircraft_state)) {
        setGuidanceForCruise(current_waypoint, aircraft_state);
        current_state = FlightState::CRUISE;
    }
    else if (shouldTransitionToLanding(aircraft_state)) {
        setGuidanceForLanding();
        current_state = FlightState::LANDING;
    }
    return guidance;
}

bool FlightStateManager::shouldTransitionToLanding(const AircraftState& state) const {
    return false;
}

bool FlightStateManager::shouldTransitionToInitialClimb(const AircraftState& aircraft_state) const {
    if (current_state != FlightState::TAKEOFF_ROLL) {
        return false;
    }
    return aircraft_state.kinematics.indicated_airspeed_knots > cessna172_VRoateSpeed;
}

bool FlightStateManager::shouldTransitionToClimbToAltitude(const AircraftState& aircraft_state) const {
    if (current_state != FlightState::INITIAL_CLIMB) {
        return false;
    }
    return aircraft_state.position.altitude_agl_ft > cessna172_InitialClimbAltitudeFt - 100;
}

bool FlightStateManager::shouldTransitionToCruise(const AircraftState& aircraft_state) const {
    if (current_state != FlightState::CLIMB_TO_ALTITUDE) {
        return false;
    }
    return aircraft_state.position.altitude_agl_ft > cessna172_CruiseAltitudeFt - 100;
}

inline MetersPerSecond FlightStateManager::knotsToMetersPerSecond(Knots knots) const {
    return knots * 0.514444;
}

void FlightStateManager::setGuidanceForTakeoffRoll(const AircraftState& aircraft_state) {
    KinematicsTargets kinematics_targets;
    kinematics_targets.vertical_speed_fpm = 900;
    kinematics_targets.indicated_airspeed_knots = cessna172_VRoateSpeed;
    guidance.kinematics_targets = kinematics_targets;

    AttitudeTargets attitude_targets;
    attitude_targets.pitch_deg = std::nullopt;
    attitude_targets.true_heading_deg = aircraft_state.attitude.true_heading;
    attitude_targets.flight_path_heading_deg = aircraft_state.attitude.flight_path_heading_deg;
    attitude_targets.flight_path_pitch_deg = std::nullopt;
    guidance.attitude_targets = attitude_targets;

    ConfigurationTargets configuration_targets;
    configuration_targets.gear_deployed = true;
    configuration_targets.flaps_position = 0.0; // Full up
    guidance.configuration_targets = configuration_targets;

    PositionTargets position_targets;
    position_targets.latitude = std::nullopt;
    position_targets.longitude = std::nullopt;
    position_targets.altitude_msl_ft = std::nullopt;
    position_targets.altitude_agl_ft = std::nullopt;
    guidance.position_targets = position_targets;

    
    
}

void FlightStateManager::setGuidanceForInitialClimb(const AircraftState& aircraft_state) {
    KinematicsTargets kinematics_targets;
    kinematics_targets.vertical_speed_fpm = cessna172_InitialClimbRateFpm;
    kinematics_targets.indicated_airspeed_knots = cessna172_CruiseSpeed;
    guidance.kinematics_targets = kinematics_targets;

    AttitudeTargets attitude_targets;
    attitude_targets.pitch_deg = std::nullopt;
    attitude_targets.true_heading_deg = aircraft_state.attitude.true_heading;
    attitude_targets.flight_path_heading_deg = aircraft_state.attitude.flight_path_heading_deg;
    attitude_targets.flight_path_pitch_deg = std::nullopt;
    guidance.attitude_targets = attitude_targets;

    ConfigurationTargets configuration_targets;
    configuration_targets.gear_deployed = true;
    configuration_targets.flaps_position = 0.0; // Full down
    guidance.configuration_targets = configuration_targets;

    PositionTargets position_targets;
    position_targets.latitude = std::nullopt;
    position_targets.longitude = std::nullopt;
    position_targets.altitude_msl_ft = std::nullopt;
    position_targets.altitude_agl_ft = cessna172_InitialClimbAltitudeFt;
    guidance.position_targets = position_targets;

    
}

void FlightStateManager::setGuidanceForClimbToAltitude(const std::optional<Waypoint>& waypoint, const AircraftState& aircraft_state) {
    KinematicsTargets kinematics_targets;
    kinematics_targets.vertical_speed_fpm = cessna172_ClimbToAltitudeRateFpm;
    kinematics_targets.indicated_airspeed_knots = cessna172_CruiseSpeed;
    guidance.kinematics_targets = kinematics_targets;

    AttitudeTargets attitude_targets;
    attitude_targets.pitch_deg = std::nullopt;
    attitude_targets.true_heading_deg = std::nullopt;
    if (waypoint.has_value()) {
        attitude_targets.flight_path_heading_deg = NavigationManager::calculateHeadingToWaypoint(aircraft_state.position.latitude, aircraft_state.position.longitude, &waypoint.value());
    }
    else {
        attitude_targets.flight_path_heading_deg = aircraft_state.attitude.flight_path_heading_deg;
    }
    attitude_targets.flight_path_pitch_deg = std::nullopt;
    guidance.attitude_targets = attitude_targets;

    ConfigurationTargets configuration_targets;
    configuration_targets.gear_deployed = false;
    configuration_targets.flaps_position = 0.0; // Full down
    guidance.configuration_targets = configuration_targets;

    PositionTargets position_targets;
    position_targets.latitude = std::nullopt;
    position_targets.longitude = std::nullopt;
    if (waypoint.has_value() && waypoint.value().altitude_msl_ft.has_value()) {
        position_targets.altitude_msl_ft = waypoint.value().altitude_msl_ft.value();
    }
    else {
        position_targets.altitude_msl_ft = cessna172_CruiseAltitudeFt;
    }
    if (waypoint.has_value() && waypoint.value().altitude_agl_ft.has_value()) {
        position_targets.altitude_agl_ft = waypoint.value().altitude_agl_ft.value();
    }
    else {
        position_targets.altitude_agl_ft = std::nullopt;
    }
    guidance.position_targets = position_targets;
}

void FlightStateManager::setGuidanceForCruise(const std::optional<Waypoint>& waypoint, const AircraftState& aircraft_state) {
    KinematicsTargets kinematics_targets;
    kinematics_targets.vertical_speed_fpm = std::nullopt;
    kinematics_targets.indicated_airspeed_knots = cessna172_CruiseSpeed;
    guidance.kinematics_targets = kinematics_targets;

    AttitudeTargets attitude_targets;
    attitude_targets.pitch_deg = std::nullopt;
    attitude_targets.true_heading_deg = std::nullopt;
    if (waypoint.has_value()) {
        attitude_targets.flight_path_heading_deg = NavigationManager::calculateHeadingToWaypoint(aircraft_state.position.latitude, aircraft_state.position.longitude, &waypoint.value());
    }
    else {
        attitude_targets.flight_path_heading_deg = aircraft_state.attitude.flight_path_heading_deg;
    }
    attitude_targets.flight_path_pitch_deg = std::nullopt;
    guidance.attitude_targets = attitude_targets;

    ConfigurationTargets configuration_targets;
    configuration_targets.gear_deployed = false;
    configuration_targets.flaps_position = 0.0; // Full down
    guidance.configuration_targets = configuration_targets;

    PositionTargets position_targets;
    position_targets.latitude = std::nullopt;
    position_targets.longitude = std::nullopt;
    if (waypoint.has_value() && waypoint.value().altitude_msl_ft.has_value()) {
        position_targets.altitude_msl_ft = waypoint.value().altitude_msl_ft.value();
    }
    else {
        position_targets.altitude_msl_ft = cessna172_CruiseAltitudeFt;
    }
    if (waypoint.has_value() && waypoint.value().altitude_agl_ft.has_value()) {
        position_targets.altitude_agl_ft = waypoint.value().altitude_agl_ft.value();
    }
    else {
        position_targets.altitude_agl_ft = std::nullopt;
    }
    guidance.position_targets = position_targets;
}