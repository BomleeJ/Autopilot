#pragma once
#include "Types.h"
#include "XPLMUtilities.h"
#include <string>
#include <sstream>

struct Kinematics {
    FeetPerMinute vertical_speed_fpm;
    Knots indicated_airspeed_knots;
    MetersPerSecond ground_speed_mps;
};

struct Attitude {
    Degrees pitch_deg; //  PITCH
    Degrees true_heading;
    Degrees roll_deg;
    Degrees flight_path_heading_deg; // True Heading of FLIGHT PATH
    Degrees flight_path_pitch_deg; // Pitch of FLIGHT PATH
};

struct AircraftConfiguration {
    bool gear_deployed;
    bool on_ground;
    ControlPosition flaps_position;
};

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

    void debugPrint() const {
        std::ostringstream oss;
        oss << "AircraftState:" << std::endl;

        oss << "  Kinematics:" << std::endl;
        oss << "    vertical_speed_fpm: " << kinematics.vertical_speed_fpm << std::endl;
        oss << "    indicated_airspeed_knots: " << kinematics.indicated_airspeed_knots << std::endl;
        oss << "    ground_speed_mps: " << kinematics.ground_speed_mps << std::endl;

        oss << "  Attitude:" << std::endl;
        oss << "    pitch_deg: " << attitude.pitch_deg << std::endl;
        oss << "    true_heading: " << attitude.true_heading << std::endl;
        oss << "    roll_deg: " << attitude.roll_deg << std::endl;
        oss << "    flight_path_heading_deg: " << attitude.flight_path_heading_deg << std::endl;
        oss << "    flight_path_pitch_deg: " << attitude.flight_path_pitch_deg << std::endl;

        oss << "  AircraftConfiguration:" << std::endl;
        oss << "    gear_deployed: " << (configuration.gear_deployed ? "true" : "false") << std::endl;
        oss << "    on_ground: " << (configuration.on_ground ? "true" : "false") << std::endl;
        oss << "    flaps_position: " << configuration.flaps_position << std::endl;

        oss << "  Actuators:" << std::endl;
        oss << "    throttle_position: " << actuators.throttle_position << std::endl;
        oss << "    yoke_pitch_position: " << actuators.yoke_pitch_position << std::endl;
        oss << "    yoke_heading_position: " << actuators.yoke_heading_position << std::endl;
        oss << "    yoke_roll_position: " << actuators.yoke_roll_position << std::endl;

        oss << "  Position:" << std::endl;
        oss << "    latitude: " << position.latitude << std::endl;
        oss << "    longitude: " << position.longitude << std::endl;
        oss << "    altitude_msl_ft: " << position.altitude_msl_ft << std::endl;
        oss << "    altitude_agl_ft: " << position.altitude_agl_ft << std::endl;

        std::string str = oss.str();
        XPLMDebugString(str.c_str());
    }
};
