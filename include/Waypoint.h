#pragma once

#include "Types.h"
#include <optional>
#include <vector>
#include "AircraftState.h"
#include <optional>
/*
Navigation Manager

gets Flight Status and Aircraft State

Depends on that it will either return a waypoint or a null pointer

*/

struct Waypoint {
    Latitude latitude;
    Longitude longitude;
    Feet altitude_msl_ft;
    Feet altitude_agl_ft;

    Waypoint(
        Latitude in_latitude, 
        Longitude in_longitude, 
        Feet in_altitude_msl_ft, 
        Feet in_altitude_agl_ft
    );
};

class NavigationManager {
    private:
    std::vector<Waypoint> waypoints;
    size_t currWaypointIdx;
    bool activated;
    
    size_t getClosestWaypointIdx(Latitude latitude, Longitude longitude);
    bool isCompleted();
    
    Kilometers calculateDistanceToWaypoint(Latitude aircraftLatitude, Longitude aircraftLongitude, const Waypoint* destinationWaypoint = nullptr) const;
    
    public:
    NavigationManager(const std::string& filename);
    std::optional<Waypoint> getCurrentWaypoint(const AircraftState& aircraft_state);
    static Degrees calculateHeadingToWaypoint(Latitude latitude, Longitude longitude, const Waypoint* destinationWaypoint);

};
