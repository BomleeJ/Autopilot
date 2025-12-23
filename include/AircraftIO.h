#include "Types.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <string>
#include "XPLMDataAccess.h"
using json = nlohmann::json;

class AircraftIO {
    json dataref_schema;

    std::string getDataRefString(const std::string& primary_key, const std::string& secondary_key) const;
    void loadJsonFile(const std::string& filename);
    public:

    template <typename T>
    T getDataRefValue(const std::string& primary_key, const std::string& secondary_key) const;
    
    template <typename T>
    T setDataRefValue(const std::string& primary_key, const std::string& secondary_key) const;
    
};