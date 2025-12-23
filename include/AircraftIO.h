#include "Types.h"
#include "nlohmann/json.hpp"
#include <string>
#include <map>
#include "XPLMDataAccess.h"

using json = nlohmann::json;

class AircraftIO {
    json dataref_schema;
    std::map<std::string, XPLMDataRef> DataRefCache;

    XPLMDataRef getDataRefPointer(const std::string& primary_key, const std::string& secondary_key) const;
    void loadJsonFile(const std::string& filename);
    public:

    template <typename T>
    T getDataRefValue(const std::string& primary_key, const std::string& secondary_key);
    
    template <typename T>
    T setDataRefValue(const std::string& primary_key, const std::string& secondary_key);
    
};