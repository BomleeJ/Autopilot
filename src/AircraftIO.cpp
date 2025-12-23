#include "AircraftIO.h"
#include "Types.h"
#include "nlohmann/json.hpp"
#include "XPLMProcessing.h"
#include <type_traits>
#include <fstream>
#include <string>
#include <map>


XPLMDataRef AircraftIO::getDataRefString(const std::string& primary_key, const std::string& secondary_key) 
{
   /*TODO MAYBE PUT IN TRY / CATCH block*/
    std::string dataref_string = dataref_schema.at(primary_key)
                                    .at(secondary_key)
                                    .get<std::string>();
    
    XPLMDataRef opaqueDataRef;
    auto it = DataRefCache.find(dataref_string);
    if (it == DataRefCache.end())
    {
        opaqueDataRef = XPLMFindDataRef(dataref_string.c_str());
    }
    else
    {
        opaqueDataRef = *it;
    }
    return opaqueDataRef;
    
}

void AircraftIO::loadJsonFile(const std::string& primary_key, const std::string& secondary_key) 
{
    // Reads Json and loads datarefs into memory
    std::ifstream f(filename);
    dataref_schema = json::parse(f); 
}

template <typename T>
T AircraftIO::getDataRefValue(const std::string& primary_key, const std::string& secondary_key) 
{
    // TEST FOR CONST CORRECTNESS
    XPLMDataRef dataref = getDataRefPointer(primary_key, secondary_key)

    if constexpr (std::is_same<T, float>)
    {
        return XPLMGetDataf(dataref);
    }
    else if (std::is_same<T, double>)
    {
        return XPLMGetDatad(dataref);
    }
    elif (std::is_same<T, int>)
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
T AircraftIO::setDataRefValue(const std::string& primary_key, const std::string& secondary_key, T inValue)
{
    XPLMDataRef dataref = getDataRefPointer(primary_key, secondary_key);

    if constexpr (std::is_same<T, float>)
    {
        XPLMSetDataf(dataref, inValue);
    }
    else if (std::is_same<T, double>)
    {
        XPLMSetDatad(dataref, inValue);
    }
    else if (std::is_same<T, int>)
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