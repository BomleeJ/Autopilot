#pragma once

#include "Types.h"
#include "AircraftState.h"
#include "Waypoint.h"
#include <optional>
#include <string>

struct NavigationDisplayData {
    std::optional<Waypoint> next_waypoint;
    std::optional<Waypoint> destination_waypoint;
    Kilometers distance_to_next_waypoint_km;
    Kilometers distance_to_destination_km;
    float estimated_time_hours;  // Will be converted to HH:MM format
    bool has_valid_data;
};

class INavigationDataProvider {
public:
    virtual ~INavigationDataProvider() = default;
    virtual NavigationDisplayData getDisplayData(const AircraftState& aircraft_state) = 0;
};

class NavigationDataProvider : public INavigationDataProvider {
private:
    NavigationManager& navigation_manager;
    
    float calculateEstimatedTime(Kilometers distance_km, MetersPerSecond ground_speed_mps) const;
    
public:
    NavigationDataProvider(NavigationManager& nav_manager);
    NavigationDisplayData getDisplayData(const AircraftState& aircraft_state) override;
};
