#include "Types.h"

struct Kinematics {
    FeetPerMinute vertical_speed_fpm;
    MetersPerSecond true_airspeed_mps;
    MetersPerSecond ground_speed_mps;
};

struct Attitude {
    Degrees pitch_deg; // True pitch
    Degrees roll_deg;
    Degrees true_heading_deg;
    Degrees vpath_pitch;
};

struct AircraftConfiguration {
    bool gear_deployed;
    bool on_ground;
    ControlPosition flaps_position;
};

/*Should these be in AircraftState? */
struct Actuators {
    ControlPosition throttle_position;
    ControlPosition yoke_pitch_position;
    ControlPosition yoke_heading_position;
    ControlPosition yoke_roll_position;

};

struct Position {
    Latitude latitude;
    Longitude longitude;
    Feet altitude_msl_ft;
    Feet altitude_agl_ft;
};

struct AircraftState {
    Kinematics kinematics;
    Attitude attitude;
    AircraftConfiguration configuration;
    Actuators actuators;
    Position position;
};