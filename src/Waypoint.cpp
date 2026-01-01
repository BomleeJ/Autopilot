#include "Waypoint.h"
#include "XPLMUtilities.h"
#include <exception>
#include <stdexcept>
#include "JsonLoader.h"
#include "nlohmann/json.hpp"
#include "Types.h"
#include <cmath>
#include <algorithm>
#include <optional>


using json = nlohmann::json;
Waypoint::Waypoint(
    Latitude in_latitude, 
    Longitude in_longitude, 
    Feet in_altitude_msl_ft, 
    Feet in_altitude_agl_ft
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
    if (altitude_msl_ft < -3000.0f) {
        throw std::invalid_argument("Altitude MSL must be greater than -3000 feet");
    }
    if (altitude_agl_ft < 0.0f) {
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

NavigationManager::NavigationManager(const std::string& filename):
currWaypointIdx(0),
activated(false)
{
    json waypoints_json = JsonLoader::loadJsonFile(filename);
    for (const auto& waypoint : waypoints_json) {
        if (waypoint.contains("LatDecimal") && waypoint.contains("LongDecimal") && waypoint.contains("AltitudeMSL") && waypoint.contains("AltitudeAGL")) 
        {
            
            Feet altitude_msl_ft = waypoint["AltitudeMSL"].is_null() ? 8500.0f : waypoint["AltitudeMSL"].get<Feet>();
            Feet altitude_agl_ft = waypoint["AltitudeAGL"].is_null() ? 2000.0f : waypoint["AltitudeAGL"].get<Feet>();
            XPLMDebugString("WAYPOINT: Altitude MSL: ");
            XPLMDebugString(std::to_string(altitude_msl_ft).c_str());
            XPLMDebugString("\nWAYPOINT: Altitude AGL: ");
            XPLMDebugString(std::to_string(altitude_agl_ft).c_str());
            XPLMDebugString("\n");
            waypoints.push_back(Waypoint(waypoint["LatDecimal"], waypoint["LongDecimal"], altitude_msl_ft, altitude_agl_ft));
        }
        else
        {
            XPLMDebugString("ERROR: Waypoint constructor: navigation.json does not contain LatDecimal or LongDecimal\n");
            XPLMDebugString("\n");
            exit(1);
        }
        
    }

    if (waypoints.empty()) {
        XPLMDebugString("ERROR: NavigationManager constructor: waypoints is empty\n");
        exit(1);
    }

}

size_t NavigationManager::getClosestWaypointIdx(Latitude latitude, Longitude longitude)
{
    auto it = std::min_element(waypoints.begin(), waypoints.end(), [this, latitude, longitude](const Waypoint& A, const Waypoint& B){
        return calculateDistanceToWaypoint(latitude, longitude, &A) < calculateDistanceToWaypoint(latitude, longitude, &B);
    });
    return std::distance(waypoints.begin(), it);
}

inline bool NavigationManager::isCompleted()
{
    return currWaypointIdx >= waypoints.size();
}

std::optional<Waypoint> NavigationManager::getCurrentWaypoint(const AircraftState& aircraft_state)
{
    Latitude latitude = aircraft_state.position.latitude;
    Longitude longitude = aircraft_state.position.longitude;
    
    if (!activated) {
        currWaypointIdx = getClosestWaypointIdx(latitude, longitude);
        activated = true;
    }

    if (calculateDistanceToWaypoint(latitude, longitude, &waypoints[currWaypointIdx]) < 1.0f) {
        currWaypointIdx++;

        if (isCompleted()) 
        {
            return std::nullopt;
        }
    }
    
    return waypoints[currWaypointIdx];
}

Degrees NavigationManager::calculateHeadingToWaypoint(Latitude aircraftLatitude, Longitude aircraftLongitude, const Waypoint* destinationWaypoint)
{
    if (destinationWaypoint == nullptr) {
        throw std::invalid_argument("Destination waypoint is nullptr");
        XPLMDebugString("ERROR: NavigationManager::calculateHeadingToWaypoint: Destination waypoint is nullptr\n");
        exit(1);
    }

    const float PI = 3.14159265358979323846f;
    const float DEG_TO_RAD = PI / 180.0f;

    const float aircraftLatitudeRad    = aircraftLatitude    * DEG_TO_RAD;
    const float aircraftLongitudeRad   = aircraftLongitude   * DEG_TO_RAD;
    const float waypointLatitudeRad    = destinationWaypoint->latitude  * DEG_TO_RAD;
    const float waypointLongitudeRad   = destinationWaypoint->longitude * DEG_TO_RAD;

    const float deltaLatitude  = waypointLatitudeRad  - aircraftLatitudeRad;
    const float deltaLongitude = waypointLongitudeRad - aircraftLongitudeRad;

    const float y = std::sin(deltaLongitude) * std::cos(waypointLatitudeRad);

    const float x = 
        (std::cos(aircraftLatitudeRad) * std::sin(waypointLatitudeRad)) 
      - (std::sin(aircraftLatitudeRad) * std::cos(waypointLatitudeRad) * std::cos(deltaLongitude));

    const float heading   = std::atan2(y, x);
    // Convert heading (in radians) to degrees and normalize to [0, 360)
    float headingDeg = heading * 180.0f / PI;
    headingDeg = std::fmod(headingDeg + 360.0f, 360.0f); // ensure [0,360)

    return headingDeg;
}

Kilometers NavigationManager::calculateDistanceToWaypoint(Latitude aircraftLatitude, Longitude aircraftLongitude, const Waypoint* destinationWaypoint) const
{
    if (destinationWaypoint == nullptr) {
        destinationWaypoint = &waypoints[currWaypointIdx];
    }

    const float EARTH_RADIUS_KM = 6371.0f;
    const float PI = 3.14159265358979323846f;
    const float DEG_TO_RAD = PI / 180.0f;

    float aircraftLatitudeRad = aircraftLatitude * DEG_TO_RAD;
    float aircraftLongitudeRad = aircraftLongitude * DEG_TO_RAD;
    float waypointLatitudeRad = destinationWaypoint->latitude * DEG_TO_RAD;
    float waypointLongitudeRad = destinationWaypoint->longitude * DEG_TO_RAD;

    float deltaLatitude = waypointLatitudeRad - aircraftLatitudeRad;
    float deltaLongitude = waypointLongitudeRad - aircraftLongitudeRad;

    float haversineA = std::sin(deltaLatitude / 2.0f) * std::sin(deltaLatitude / 2.0f) +
                       std::cos(aircraftLatitudeRad) * std::cos(waypointLatitudeRad) *
                       std::sin(deltaLongitude / 2.0f) * std::sin(deltaLongitude / 2.0f);
    float angularDistance = 2.0f * std::atan2(std::sqrt(haversineA), std::sqrt(1.0f - haversineA));
    float distanceKilometers = EARTH_RADIUS_KM * angularDistance;
    return distanceKilometers;
}