#include "Waypoint.h"
#include "XPLMUtilities.h"
#include <exception>
#include <stdexcept>

Waypoint::Waypoint(
    Latitude in_latitude, 
    Longitude in_longitude, 
    std::optional<Feet> in_altitude_msl_ft = std::nullopt, 
    std::optional<Feet> in_altitude_agl_ft = std::nullopt
) : 
latitude(in_latitude), 
longitude(in_longitude), 
altitude_msl_ft(in_altitude_msl_ft),
altitude_agl_ft(in_altitude_agl_ft)
{
    try 
    {
    if (in_latitude < -90.0 || in_latitude > 90.0) {
        throw std::invalid_argument("Latitude must be between -90 and 90 degrees");
    }
    if (in_longitude < -180.0 || in_longitude > 180.0) {
        throw std::invalid_argument("Longitude must be between -180 and 180 degrees");
    }
    if (in_altitude_msl_ft.has_value() && in_altitude_msl_ft.value() < -3000.0f) {
        throw std::invalid_argument("Altitude MSL must be greater than -3000 feet");
    }
    if (in_altitude_agl_ft.has_value() && in_altitude_agl_ft.value() < 0.0f) {
        throw std::invalid_argument("Altitude AGL must be greater than 0 feet");
    }
    } 
    catch (const std::invalid_argument& e) {
        XPLMDebugString("ERROR: Waypoint constructor: ");
        XPLMDebugString(e.what());
        XPLMDebugString("\n");
        exit(1);
    }
}