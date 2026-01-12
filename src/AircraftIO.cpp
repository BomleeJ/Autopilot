#include "AircraftIO.h"
#include "nlohmann/json.hpp"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include <fstream>
#include <string>
#include <map>
#include <exception>
#include "JsonLoader.h"
#include "AircraftState.h"


AircraftIO::AircraftIO(const std::string& filename)
{
    try {
        dataref_schema = JsonLoader::loadJsonFile(filename);
        XPLMDebugString("LOADUP: Json Parsed No issues \n");
        
    } 
    catch (const std::exception& e)
    {
        XPLMDebugString("ERROR: LOADING json: ");
        XPLMDebugString(e.what());
        XPLMDebugString("\n");
        exit(1);
    }
    
}
/* NOT COMPLETED YET */
AircraftState AircraftIO::getAircraftState()
{
    AircraftState aircraft_state;
    updateAircraftState(aircraft_state);
    return aircraft_state;
}

void AircraftIO::updateAircraftState(AircraftState& aircraft_state)
{
    updateKinematics(aircraft_state);
    updateAttitude(aircraft_state);
    updateConfiguration(aircraft_state);
    updateActuators(aircraft_state);
    updatePosition(aircraft_state);
}

void AircraftIO::updateKinematics(AircraftState& aircraft_state)
{
    aircraft_state.kinematics.vertical_speed_fpm = getDataRefValue<float>("speed", "vertical_speed_fpm");
    aircraft_state.kinematics.indicated_airspeed_knots = getDataRefValue<float>("speed", "indicated_airspeed_knots");
    aircraft_state.kinematics.ground_speed_mps = getDataRefValue<float>("speed", "ground_speed_mps");
}

void AircraftIO::updateAttitude(AircraftState& aircraft_state)
{
    aircraft_state.attitude.pitch_deg = getDataRefValue<float>("attitude", "pitch_deg");
    aircraft_state.attitude.true_heading = getDataRefValue<float>("attitude", "true_heading_deg");
    aircraft_state.attitude.roll_deg = getDataRefValue<float>("attitude", "roll_deg");
    aircraft_state.attitude.flight_path_heading_deg = getDataRefValue<float>("flight_path", "hpath_deg");
    aircraft_state.attitude.flight_path_pitch_deg = getDataRefValue<float>("flight_path", "vpath_deg");
}

void AircraftIO::updateConfiguration(AircraftState& aircraft_state)
{
    aircraft_state.configuration.gear_deployed = (bool) getDataRefValue<int>("gear", "gear_deployed");
    aircraft_state.configuration.on_ground = (bool) getDataRefValue<int>("gear", "on_ground");
    aircraft_state.configuration.flaps_position = (float)getDataRefValue<float>("cockpit_controls", "flap_ratio");
}

void AircraftIO::updateActuators(AircraftState& aircraft_state)
{
    aircraft_state.actuators.throttle_position = getDataRefValue<float>("cockpit_controls", "throttle_ratio");
    aircraft_state.actuators.yoke_pitch_position = getDataRefValue<float>("cockpit_controls", "yoke_pitch_ratio");
    aircraft_state.actuators.yoke_heading_position = getDataRefValue<float>("cockpit_controls", "yoke_heading_ratio");
    aircraft_state.actuators.yoke_roll_position = getDataRefValue<float>("cockpit_controls", "yoke_roll_ratio");
}

void AircraftIO::updatePosition(AircraftState& aircraft_state)
{
    aircraft_state.position.latitude = getDataRefValue<float>("position", "latitude_deg");
    aircraft_state.position.longitude = getDataRefValue<float>("position", "longitude_deg");
    aircraft_state.position.altitude_msl_ft = getDataRefValue<float>("altitude", "msl_ft");
    aircraft_state.position.altitude_agl_ft = getDataRefValue<float>("altitude", "agl_ft") * 3.28084f;
}

XPLMDataRef AircraftIO::getDataRefPointer(const std::string& primary_key, const std::string& secondary_key) 
{
   /*TODO MAYBE PUT IN TRY / CATCH block*/
   std::string dataref_string;
   try {
    dataref_string = dataref_schema.at(primary_key)
                                    .at(secondary_key)
                                    .get<std::string>();
    } catch (std::exception& e)
    {
        XPLMDebugString("ERROR AircrafIO.cpp");
        XPLMDebugString(e.what());
        XPLMDebugString("\n");
        exit(1);
    }                           
    
    XPLMDataRef opaqueDataRef;
    auto it = DataRefCache.find(dataref_string);
    if (it == DataRefCache.end())
    {
        opaqueDataRef = XPLMFindDataRef(dataref_string.c_str());
        DataRefCache[dataref_string] = opaqueDataRef;
    }
    else
    {
        opaqueDataRef = (*it).second;
    }
    return opaqueDataRef;
    
}

void AircraftIO::setThrottlePosition(float throttle_position)
{
    setDataRefValue<float>("cockpit_controls", "throttle_ratio", throttle_position);
}

void AircraftIO::setYokePitchPosition(float yoke_pitch_position)
{
    setDataRefValue<float>("cockpit_controls", "yoke_pitch_ratio", yoke_pitch_position);
}

void AircraftIO::setYokeHeadingPosition(float yoke_heading_position)
{
    setDataRefValue<float>("cockpit_controls", "yoke_heading_ratio", yoke_heading_position);
}

void AircraftIO::setYokeRollPosition(float yoke_roll_position)
{
    setDataRefValue<float>("cockpit_controls", "yoke_roll_ratio", yoke_roll_position);
}

