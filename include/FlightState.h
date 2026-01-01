#pragma once
#include "Types.h"
#include "AircraftGuidance.h"
#include "Waypoint.h"
#include <vector>
#include <optional>

class NavigationManager;
struct AircraftState; // Forward declaration

class FlightStateManager {
    private:
    AircraftGuidance guidance;

    public:
    std::optional<AircraftGuidance> getGuidance(const AircraftState& aircraft_state, NavigationManager& navigation_manager);
};