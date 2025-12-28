#pragma once
#include "Types.h"
#include "AircraftGuidance.h"
#include "Waypoint.h"
#include <vector>

class NavigationManager;
struct AircraftState; // Forward declaration

enum class FlightState {
    TAKEOFF_ROLL,
    INITIAL_CLIMB,
    CLIMB_TO_ALTITUDE,
    CRUISE,
    LANDING
};
class FlightStateManager {
    private:
    AircraftGuidance guidance;
    FlightState current_state;

    bool shouldTransitionToInitialClimb(const AircraftState& state) const;
    bool shouldTransitionToClimbToAltitude(const AircraftState& state) const;
    bool shouldTransitionToCruise(const AircraftState& state) const;
    bool shouldTransitionToLanding(const AircraftState& state) const;
    
    void setGuidanceForTakeoffRoll(const AircraftState& aircraft_state);
    void setGuidanceForInitialClimb(const AircraftState& aircraft_state);
    void setGuidanceForClimbToAltitude(const std::optional<Waypoint>& waypoint, const AircraftState& aircraft_state);
    void setGuidanceForCruise(const std::optional<Waypoint>& waypoint, const AircraftState& aircraft_state);
    void setGuidanceForLanding();

    MetersPerSecond knotsToMetersPerSecond(Knots knots) const;


    public:
    FlightStateManager(const AircraftState& aircraft_state);
    AircraftGuidance getGuidance(const AircraftState& aircraft_state, NavigationManager& navigation_manager);
};