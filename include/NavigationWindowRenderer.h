#pragma once

#include "NavigationDataProvider.h"
#include "XPLMGraphics.h"
#include <string>

class NavigationWindowRenderer {
private:
    static constexpr XPLMFontID FONT_ID = 0;  // xplmFont_Basic = 0
    static constexpr float TEXT_COLOR[3] = {1.0f, 1.0f, 1.0f};  // White
    static constexpr float LABEL_COLOR[3] = {0.8f, 0.8f, 0.8f};  // Light gray
    
    int char_width;
    int char_height;
    int line_height;
    
    void initializeFontMetrics();
    void drawText(int x, int y, const char* text);
    void drawLabelValue(int x, int y, const char* label, const char* value);
    std::string formatWaypointInfo(const Waypoint& wp);
    std::string formatTime(float hours);
    std::string formatDistance(Kilometers km);
    
public:
    NavigationWindowRenderer();
    void render(const NavigationDisplayData& data, int windowWidth, int windowHeight);
};
