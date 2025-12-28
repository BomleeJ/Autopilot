#include <string>
#include "nlohmann/json.hpp"
#include <fstream>
#include "XPLMUtilities.h"
#include <exception>
#include <stdexcept>

using json = nlohmann::json;
namespace JsonLoader {

    json loadJsonFile(const std::string& filename)
    {
        try {
            std::ifstream f(filename);
            return json::parse(f);
        } catch (std::exception& e) {
            XPLMDebugString("ERROR: JsonLoader.h: ");
            XPLMDebugString(filename.c_str());
            XPLMDebugString(e.what());
            XPLMDebugString("\n");
            exit(1);
        }

    }
};