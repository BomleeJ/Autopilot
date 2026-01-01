#include "FlightState.h"
#include "AircraftGuidance.h"
#include "AircraftState.h"
#include <optional>
#include "Types.h"
#include "Waypoint.h"
#include <optional>

Knots cessna172_VRoateSpeed = 55;
Knots cessna172_CruiseSpeed = 110;
FeetPerMinute cessna172_InitialClimbRateFpm = 900;
FeetPerMinute cessna172_ClimbToAltitudeRateFpm = 700;
Feet cessna172_InitialClimbAltitudeFt = 500;
Feet cessna172_CruiseAltitudeFt = 8500;

std::optional<AircraftGuidance> FlightStateManager::getGuidance(const AircraftState& aircraft_state, NavigationManager& navigation_manager) 
{
    std::optional<Waypoint> current_waypoint = navigation_manager.getCurrentWaypoint(aircraft_state);

    KinematicsTargets kinematics_targets;
    kinematics_targets.vertical_speed_fpm = std::nullopt;
    kinematics_targets.indicated_airspeed_knots = cessna172_CruiseSpeed;
    guidance.kinematics_targets = kinematics_targets;

    AttitudeTargets attitude_targets;
    attitude_targets.pitch_deg = std::nullopt;
    attitude_targets.flight_path_pitch_deg = std::nullopt;
    guidance.attitude_targets = attitude_targets;

    ConfigurationTargets configuration_targets;
    configuration_targets.gear_deployed = false;
    configuration_targets.flaps_position = 0.0;
    guidance.configuration_targets = configuration_targets;

    PositionTargets position_targets;
    position_targets.latitude = std::nullopt;
    position_targets.longitude = std::nullopt;

    if (current_waypoint.has_value()) {
        Waypoint waypoint_value = current_waypoint.value();
        position_targets.altitude_msl_ft = waypoint_value.altitude_msl_ft;
        position_targets.altitude_agl_ft = waypoint_value.altitude_agl_ft;
        guidance.position_targets = position_targets;

        guidance.attitude_targets.true_heading_deg = NavigationManager::calculateHeadingToWaypoint(aircraft_state.position.latitude, aircraft_state.position.longitude, &waypoint_value);
        guidance.attitude_targets.flight_path_heading_deg = NavigationManager::calculateHeadingToWaypoint(aircraft_state.position.latitude, aircraft_state.position.longitude, &waypoint_value);
        
    }
    else {
        return std::nullopt;
    }
    return guidance;
}
