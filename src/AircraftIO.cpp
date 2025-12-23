#include "AircraftIO.h"
#include "Logger.h"
#include "nlohmann/json.hpp"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include <fstream>
#include <string>
#include <map>
#include <exception>

AircraftIO::AircraftIO(const std::string& filename)
{
    try {
        loadJsonFile(filename);
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

void AircraftIO::loadJsonFile(const std::string& filename) 
{
    // Reads Json and loads datarefs into memory
    std::ifstream f(filename);
    dataref_schema = json::parse(f); 
}

