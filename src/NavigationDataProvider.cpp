#include "NavigationDataProvider.h"
#include "Waypoint.h"
#include <algorithm>

NavigationDataProvider::NavigationDataProvider(NavigationManager& nav_manager) :
    navigation_manager(nav_manager)
{
}

float NavigationDataProvider::calculateEstimatedTime(Kilometers distance_km, MetersPerSecond ground_speed_mps) const
{
    if (ground_speed_mps <= 0.0f) {
        return -1.0f;  // Invalid time indicator
    }
    
    // Convert m/s to km/h: multiply by 3.6
    float ground_speed_kmh = ground_speed_mps * 3.6f;
    
    // Calculate time in hours
    float time_hours = distance_km / ground_speed_kmh;
    
    return time_hours;
}

NavigationDisplayData NavigationDataProvider::getDisplayData(const AircraftState& aircraft_state)
{
    NavigationDisplayData data;
    data.has_valid_data = false;
    data.estimated_time_hours = -1.0f;
    data.distance_to_next_waypoint_km = 0.0f;
    data.distance_to_destination_km = 0.0f;
    
    // Get current waypoint (this is the "next" waypoint we're heading to)
    data.next_waypoint = navigation_manager.getCurrentWaypoint(aircraft_state);
    
    // Calculate distance to next waypoint
    if (data.next_waypoint.has_value()) {
        data.distance_to_next_waypoint_km = navigation_manager.calculateDistanceToCurrentWaypoint(
            aircraft_state.position.latitude,
            aircraft_state.position.longitude
        );
    }
    
    // Get destination waypoint
    data.destination_waypoint = navigation_manager.getDestinationWaypoint();
    
    if (!data.destination_waypoint.has_value()) {
        return data;  // No destination, return empty data
    }
    
    // Calculate distance to destination
    data.distance_to_destination_km = navigation_manager.calculateDistanceToDestination(
        aircraft_state.position.latitude,
        aircraft_state.position.longitude
    );
    
    // Calculate estimated time
    data.estimated_time_hours = calculateEstimatedTime(
        data.distance_to_destination_km,
        aircraft_state.kinematics.ground_speed_mps
    );
    
    data.has_valid_data = true;
    return data;
}
