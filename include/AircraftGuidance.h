#pragma once
#include "Types.h"
#include <optional>
/*WHERE WE WANT AIRCRAFT TO GO*/

struct KinematicsTargets {
    std::optional<FeetPerMinute> vertical_speed_fpm;
    std::optional<MetersPerSecond> indicated_airspeed_mps;
};

struct AttitudeTargets {
    std::optional<Degrees> pitch_deg;
    std::optional<Degrees> true_heading_deg;
    std::optional<Degrees> flight_path_heading_deg;
    std::optional<Degrees> flight_path_pitch_deg;
};

struct ConfigurationTargets {
   bool gear_deployed;
   std::optional<ControlPosition> flaps_position;
};

struct PositionTargets {
    std::optional<Latitude> latitude;
    std::optional<Longitude> longitude;
    std::optional<Feet> altitude_msl_ft;
    std::optional<Feet> altitude_agl_ft;
};
struct AircraftGuidance {
    KinematicsTargets kinematics_targets;
    AttitudeTargets attitude_targets;
    ConfigurationTargets configuration_targets;
    PositionTargets position_targets;
};

