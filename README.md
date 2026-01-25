# X-Plane C172 Autopilot (XPLM300/301)

An experimental X-Plane plugin that reads aircraft state via DataRefs, computes guidance to a sequence of waypoints, and drives basic control outputs (throttle + roll/heading + pitch) using PID-style controllers. It also renders a small in-sim navigation window showing next/destination waypoint, distance, and ETA.

## Technical overview

### Runtime data flow
- **X-Plane flight loop callback**: `test.cpp` registers a flight loop and runs the control loop.
- **State ingestion**: `AircraftIO` reads a structured set of DataRefs (defined in `datarefs.json`) and produces an `AircraftState`.
- **Guidance generation**: `FlightStateManager::getGuidance(...)` produces an `AircraftGuidance` (targets for kinematics/attitude/config/position) based on the current waypoint from `NavigationManager`.
- **Control**:
  - **Throttle**: `ThrottlePIDController` drives `sim/cockpit2/engine/actuators/throttle_ratio_all` toward a target IAS.
  - **Roll/heading**: `HeadingPIDController` computes a bank/roll command (yoke roll) to reduce heading error toward the waypoint bearing.
  - **Pitch/altitude**: `PitchPIDController` uses an altitude→vspeed target mapping and a vspeed PID to compute yoke pitch.
- **UI**: `NavigationWindow` + `NavigationDataProvider` + `NavigationWindowRenderer` build and draw a floating window with waypoint and ETA info using XPLM drawing APIs.

### Key modules
- **`test.cpp`**
  - Plugin entry points (`XPluginStart`, etc.)
  - Flight loop scheduling and main control loop
  - Global singletons for IO, navigation, controllers, and UI
- **`include/AircraftState.h`**
  - Core state struct (`Kinematics`, `Attitude`, `Position`, `Actuators`, `AircraftConfiguration`)
- **`include/AircraftGuidance.h`**
  - Target struct used by controllers (`KinematicsTargets`, `AttitudeTargets`, `PositionTargets`, etc.)
- **`src/AircraftIO.cpp` / `include/AircraftIO.h`**
  - Loads `datarefs.json`, caches `XPLMDataRef` handles, reads and writes values via `XPLMGetData*`/`XPLMSetData*`
- **`src/Waypoint.cpp` / `include/Waypoint.h`**
  - `Waypoint` validation
  - `NavigationManager`: loads `navigation.json`, selects/advances waypoints, computes Haversine distance and bearing
- **`src/PID.cpp` / `include/PID.h`**
  - PID primitives + throttle/heading/pitch controllers
- **`src/NavigationWindow.cpp` / `include/NavigationWindow.h`**
  - Creates an `XPLMCreateWindowEx` floating window and feeds it the current `AircraftState`
- **`src/NavigationDataProvider.cpp` / `include/NavigationDataProvider.h`**
  - Produces `NavigationDisplayData` (distances + ETA) from `AircraftState` + `NavigationManager`
- **`src/NavigationWindowRenderer.cpp` / `include/NavigationWindowRenderer.h`**
  - Renders a translucent background and formatted text via `XPLMDrawString` / `XPLMDrawTranslucentDarkBox`

## Build & install (current Makefile)

### Prerequisites
- **X-Plane SDK**: vendored under `SDK/` (headers + platform libraries).
- **C++17 compiler**: Makefile uses `clang++`.
- **nlohmann/json**: Makefile expects it via Homebrew:

```bash
brew install nlohmann-json
```

### Build

```bash
make
```

This produces `plugin.xpl` (a macOS bundle in the current configuration).

### Install into X-Plane

```bash
make install
```

The `install` target (as currently written) copies:
- `plugin.xpl` → `~/Library/Application Support/Steam/steamapps/common/X-Plane 11/Resources/plugins`
- `datarefs.json` and `navigation.json` → `~/Library/Application Support/Steam/steamapps/common/X-Plane 11`

**Note:** The Makefile currently hard-codes the macOS framework search path and the Steam X-Plane 11 install location. If your SDK/X-Plane path differs, update:
- `LDFLAGS` (`-F.../SDK/Libraries/Mac`)
- the `install` copy destinations

## Runtime configuration files

### `datarefs.json` (DataRef schema)
`AircraftIO` expects a nested object of categories → keys → DataRef path strings.

Current top-level categories used by the code:
- **`altitude`**: `msl_ft`, `agl_ft`, `radio_alt_ft`, `landing_alt_msl_ft`
- **`attitude`**: `pitch_deg`, `roll_deg`, `true_heading_deg`
- **`flight_path`**: `hpath_deg`, `vpath_deg`
- **`speed`**: `true_airspeed_mps`, `ground_speed_mps`, `vertical_speed_fpm`, `indicated_airspeed_knots`
- **`position`**: `latitude_deg`, `longitude_deg`
- **`controls`**: raw control ratios (currently not all are consumed)
- **`cockpit_controls`**: `yoke_*_ratio`, `throttle_ratio`, etc. (used for readback + actuation)
- **`gear`**: gear-related state

If you rename keys or categories, you must update the corresponding reads/writes in `AircraftIO`.

### `navigation.json` (waypoint list)
`NavigationManager` expects an **array** of waypoint objects with at least:
- `LatDecimal` (number)
- `LongDecimal` (number)
- `AltitudeMSL` (number or null)
- `AltitudeAGL` (number or null)

Optional fields:
- `ID` (string)
- `Name` (string)

Null altitude values are replaced with defaults in code (`AltitudeMSL` → 8500 ft, `AltitudeAGL` → 2000 ft).

Waypoint advancement is distance-based: when the aircraft is within **1.0 km** of the current waypoint, the manager increments to the next waypoint. The initial waypoint is chosen as the **closest** waypoint to the aircraft position when the manager first activates.

## Debugging & instrumentation
- The plugin emits extensive debug output via `XPLMDebugString(...)` (viewable in X-Plane’s `Log.txt`).
- On JSON parse/load failures, the code currently logs and `exit(1)`; ensure `datarefs.json` and `navigation.json` are present where the plugin expects them.

## Compatibility notes
- The project is currently configured for **macOS x86_64** and XPLM **300/301** (`-arch x86_64`, `-DAPL=1`, `-DXPLM300`, `-DXPLM301`).
- To build on other platforms (or Apple Silicon), you’ll need to adjust the Makefile compiler flags and link settings to match the appropriate SDK libraries for your target.
