#include "NavigationWindowRenderer.h"
#include "XPLMGraphics.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstring>

NavigationWindowRenderer::NavigationWindowRenderer()
{
    initializeFontMetrics();
}

void NavigationWindowRenderer::initializeFontMetrics()
{
    XPLMGetFontDimensions(FONT_ID, &char_width, &char_height, nullptr);
    line_height = char_height + 2;  // Small gap between lines
}

void NavigationWindowRenderer::drawText(int x, int y, const char* text)
{
    float color[3] = {TEXT_COLOR[0], TEXT_COLOR[1], TEXT_COLOR[2]};
    XPLMDrawString(color, x, y, const_cast<char*>(text), nullptr, FONT_ID);
}

void NavigationWindowRenderer::drawLabelValue(int x, int y, const char* label, const char* value)
{
    // Draw label in lighter color
    float labelColor[3] = {LABEL_COLOR[0], LABEL_COLOR[1], LABEL_COLOR[2]};
    XPLMDrawString(labelColor, x, y, const_cast<char*>(label), nullptr, FONT_ID);
    
    // Calculate position for value (after label)
    int labelWidth = static_cast<int>(strlen(label)) * char_width;
    int valueX = x + labelWidth;
    
    // Draw value in white
    float textColor[3] = {TEXT_COLOR[0], TEXT_COLOR[1], TEXT_COLOR[2]};
    XPLMDrawString(textColor, valueX, y, const_cast<char*>(value), nullptr, FONT_ID);
}

std::string NavigationWindowRenderer::formatWaypointInfo(const Waypoint& wp)
{
    std::ostringstream oss;
    
    // Display name/ID if available, otherwise coordinates
    if (!wp.name.empty()) {
        oss << wp.name;
        if (!wp.id.empty() && wp.id != wp.name) {
            oss << " (" << wp.id << ")";
        }
    } else if (!wp.id.empty()) {
        oss << wp.id;
    } else {
        oss << "Waypoint";
    }
    
    return oss.str();
}

std::string NavigationWindowRenderer::formatTime(float hours)
{
    if (hours < 0.0f) {
        return "N/A";
    }
    
    int total_minutes = static_cast<int>(std::round(hours * 60.0f));
    int h = total_minutes / 60;
    int m = total_minutes % 60;
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << h << ":"
        << std::setfill('0') << std::setw(2) << m;
    
    return oss.str();
}

std::string NavigationWindowRenderer::formatDistance(Kilometers km)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << km << " km";
    return oss.str();
}

void NavigationWindowRenderer::render(const NavigationDisplayData& data, int windowWidth, int windowHeight)
{
    // Draw semi-transparent dark background for readability
    XPLMDrawTranslucentDarkBox(0, windowHeight, windowWidth, 0);
    
    if (!data.has_valid_data) {
        drawText(10, windowHeight - 20, "No navigation data available");
        return;
    }
    
    int x = 10;
    int y = windowHeight - 20;  // Start from top
    
    // Next Waypoint
    if (data.next_waypoint.has_value()) {
        const Waypoint& wp = data.next_waypoint.value();
        std::string wpInfo = formatWaypointInfo(wp);
        drawLabelValue(x, y, "Next: ", wpInfo.c_str());
        y -= line_height;
        
        std::ostringstream coord;
        coord << std::fixed << std::setprecision(6) 
              << "  Lat: " << wp.latitude << ", Lon: " << wp.longitude;
        drawText(x, y, coord.str().c_str());
        y -= line_height;
        
        std::ostringstream alt;
        alt << "  Alt: " << std::fixed << std::setprecision(0) << wp.altitude_msl_ft << " ft";
        drawText(x, y, alt.str().c_str());
        y -= line_height;
        
        // Distance to next waypoint
        std::string distanceToNextStr = formatDistance(data.distance_to_next_waypoint_km);
        drawLabelValue(x, y, "  Dist: ", distanceToNextStr.c_str());
        y -= line_height * 2;  // Extra space before next section
    } else {
        drawLabelValue(x, y, "Next: ", "N/A");
        y -= line_height * 2;
    }
    
    // Destination Waypoint
    if (data.destination_waypoint.has_value()) {
        const Waypoint& wp = data.destination_waypoint.value();
        std::string wpInfo = formatWaypointInfo(wp);
        drawLabelValue(x, y, "Dest: ", wpInfo.c_str());
        y -= line_height;
        
        std::ostringstream coord;
        coord << std::fixed << std::setprecision(6) 
              << "  Lat: " << wp.latitude << ", Lon: " << wp.longitude;
        drawText(x, y, coord.str().c_str());
        y -= line_height;
        
        std::ostringstream alt;
        alt << "  Alt: " << std::fixed << std::setprecision(0) << wp.altitude_msl_ft << " ft";
        drawText(x, y, alt.str().c_str());
        y -= line_height * 2;  // Extra space before next section
    } else {
        drawLabelValue(x, y, "Dest: ", "N/A");
        y -= line_height * 2;
    }
    
    // Distance to Destination
    std::string distanceStr = formatDistance(data.distance_to_destination_km);
    drawLabelValue(x, y, "Distance: ", distanceStr.c_str());
    y -= line_height * 2;
    
    // Estimated Time to Destination
    std::string timeStr = formatTime(data.estimated_time_hours);
    drawLabelValue(x, y, "ETA: ", timeStr.c_str());
}
