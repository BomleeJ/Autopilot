#pragma once
#include "Types.h"
#include "nlohmann/json.hpp"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include <type_traits>
#include <string>
#include "AircraftState.h"
#include <map>

using json = nlohmann::json;

class AircraftIO {
    json dataref_schema;
    std::map<std::string, XPLMDataRef> DataRefCache;

    XPLMDataRef getDataRefPointer(const std::string& primary_key, const std::string& secondary_key);
    

    void updateAircraftState(AircraftState& aircraft_state);
    void updateKinematics(AircraftState& aircraft_state);
    void updateAttitude(AircraftState& aircraft_state);
    void updateConfiguration(AircraftState& aircraft_state);
    void updateActuators(AircraftState& aircraft_state);
    void updatePosition(AircraftState& aircraft_state);

    public:

    AircraftIO(const std::string& filename);

    AircraftState getAircraftState();

    template <typename T>
    T getDataRefValue(const std::string& primary_key, const std::string& secondary_key) 
    {
        // TEST FOR CONST CORRECTNESS
        XPLMDataRef dataref = getDataRefPointer(primary_key, secondary_key);

        if constexpr (std::is_same_v<T, float>)
        {
            return XPLMGetDataf(dataref);
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            return XPLMGetDatad(dataref);
        }
        else if constexpr (std::is_same_v<T, int>)
        {
            return XPLMGetDatai(dataref);
        }
        else
        {
            // Throw
            XPLMDebugString("ERROR: getDataRefValue<T> not implemented for type T");
        }

        return 0;
    }
    
    template <typename T>
    T setDataRefValue(const std::string& primary_key, const std::string& secondary_key, T inValue)
    {
        XPLMDataRef dataref = getDataRefPointer(primary_key, secondary_key);

        if constexpr (std::is_same_v<T, float>)
        {
            XPLMSetDataf(dataref, inValue);
        }
        else if (std::is_same_v<T, double>)
        {
            XPLMSetDatad(dataref, inValue);
        }
        else if (std::is_same_v<T, int>)
        {
            XPLMSetDatai(dataref, inValue);
        }
        else
        {
            // Throw
            XPLMDebugString("ERROR: setDataRefValue<T> not implemented for type T\n");
            XPLMDebugString(("Type: " + std::string(typeid(T).name()) + "\n").c_str());
            XPLMDebugString(("Secondary Key: " + secondary_key + "\n").c_str());
        }

        return 0;
    }
    
};

