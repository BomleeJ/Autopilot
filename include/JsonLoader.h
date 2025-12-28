#pragma once
#include <string>
#include "nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;
namespace JsonLoader {
    json loadJsonFile(const std::string& filename);
};