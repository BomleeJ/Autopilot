#include "AircraftIO.h"

std::string AircraftIO::getDataRefString(const std::string& primary_key, const std::string& secondary_key) 
{
   /*TODO MAYBE PUT IN TRY / CATCH block*/
    std::string val = dataref_schema.at(primary_key)
                                    .at(secondary_key)
                                    .get<std::string>();
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
    
}

template <typename T>
T AircraftIO::setDataRefValue(const std::string& primary_key, const std::string& secondary_key)
{
    
}