#pragma once
#include "Types.h"
#include "AircraftGuidance.h"

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
    AircraftState& aircraft_state;


    bool shouldTransitionToInitialClimb(const AircraftState& state) const;
    bool shouldTransitionToClimbToAltitude(const AircraftState& state) const;
    bool shouldTransitionToCruise(const AircraftState& state) const;
    bool shouldTransitionToLanding(const AircraftState& state) const;
    
    void setGuidanceForTakeoffRoll();
    void setGuidanceForInitialClimb();
    void setGuidanceForClimbToAltitude();
    void setGuidanceForCruise();
    void setGuidanceForLanding();

    MetersPerSecond knotsToMetersPerSecond(Knots knots) const;


    public:
    FlightStateManager(AircraftState& aircraft_state);
    void updateAircraftState(const AircraftState& state);
    AircraftGuidance getGuidance(Waypoint& waypoint);
};