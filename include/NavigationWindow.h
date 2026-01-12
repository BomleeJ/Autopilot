#pragma once

#include "XPLMDisplay.h"
#include "NavigationDataProvider.h"
#include "NavigationWindowRenderer.h"
#include "AircraftState.h"
#include "Waypoint.h"
#include <memory>

class NavigationWindow {
private:
    XPLMWindowID window_id;
    std::unique_ptr<INavigationDataProvider> data_provider;
    std::unique_ptr<NavigationWindowRenderer> renderer;
    AircraftState current_aircraft_state;  // Cached state for rendering
    
    // XPLM window callbacks (static for C compatibility)
    static void drawCallback(XPLMWindowID inWindowID, void* inRefcon);
    static int mouseClickCallback(XPLMWindowID inWindowID, int x, int y, 
                                   XPLMMouseStatus inMouse, void* inRefcon);
    static XPLMCursorStatus cursorCallback(XPLMWindowID inWindowID, int x, int y, void* inRefcon);
    static int mouseWheelCallback(XPLMWindowID inWindowID, int x, int y, 
                                   int wheel, int clicks, void* inRefcon);
    static void keyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags,
                            char inVirtualKey, void* inRefcon, int losingFocus);
    
    // Instance methods called from static callbacks
    void onDraw();
    void updateAircraftState(const AircraftState& state);
    
public:
    NavigationWindow(NavigationManager& nav_manager);
    ~NavigationWindow();
    
    void setAircraftState(const AircraftState& state);
    void show();
    void hide();
    bool isVisible() const;
};
