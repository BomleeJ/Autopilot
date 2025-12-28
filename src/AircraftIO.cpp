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

