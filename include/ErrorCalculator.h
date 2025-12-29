#pragma once
#include "Types.h"
#include "AircraftState.h"
#include "AircraftGuidance.h"

namespace ErrorCalculator {

Error calculateSpeedError(const AircraftState& aircraft_state, const AircraftGuidance& guidance);
Error calculateVspeedError(const AircraftState& aircraft_state, const AircraftGuidance& guidance);
Error calculatePhiError(const AircraftState& aircraft_state, Degrees roll_deg_target);
Error calculateHeadingError(const AircraftState& aircraft_state, const AircraftGuidance& guidance);
}