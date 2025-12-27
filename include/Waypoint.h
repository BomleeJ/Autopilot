#pragma once

#include "Types.h"
#include <optional>

struct Waypoint {
    Latitude latitude;
    Longitude longitude;
    std::optional<Feet> altitude_msl_ft;
    std::optional<Feet> altitude_agl_ft;

    Waypoint(
        Latitude in_latitude, 
        Longitude in_longitude, 
        std::optional<Feet> in_altitude_msl_ft = std::nullopt, 
        std::optional<Feet> in_altitude_agl_ft = std::nullopt
    );
};