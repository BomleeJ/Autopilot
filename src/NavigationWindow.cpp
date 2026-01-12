#include "NavigationWindow.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMUtilities.h"
#include <algorithm>

NavigationWindow::NavigationWindow(NavigationManager& nav_manager) :
    window_id(nullptr),
    data_provider(std::make_unique<NavigationDataProvider>(nav_manager)),
    renderer(std::make_unique<NavigationWindowRenderer>())
{
    // Get screen bounds to position window in top-right
    int screenLeft = 0, screenTop = 0, screenRight = 1920, screenBottom = 0;
#if defined(XPLM300)
    XPLMGetScreenBoundsGlobal(&screenLeft, &screenTop, &screenRight, &screenBottom);
#endif
    
    // Window dimensions
    const int windowWidth = 450;
    const int windowHeight = 350;
    
    // Position in top-right corner with some margin
    int left = screenRight - windowWidth - 20;
    int top = screenTop - 20;
    int right = screenRight - 20;
    int bottom = top - windowHeight;
    
    // Create window structure
    XPLMCreateWindow_t windowParams;
    windowParams.structSize = sizeof(XPLMCreateWindow_t);
    windowParams.left = left;
    windowParams.top = top;
    windowParams.right = right;
    windowParams.bottom = bottom;
    windowParams.visible = 1;
    windowParams.drawWindowFunc = drawCallback;
    windowParams.handleMouseClickFunc = mouseClickCallback;
    windowParams.handleKeyFunc = keyCallback;
    windowParams.handleCursorFunc = cursorCallback;
    windowParams.handleMouseWheelFunc = mouseWheelCallback;
    windowParams.refcon = this;
#if defined(XPLM301)
    windowParams.decorateAsFloatingWindow = xplm_WindowDecorationNone;  // Simple window
#endif
#if defined(XPLM300)
    windowParams.layer = xplm_WindowLayerFloatingWindows;
#endif
#if defined(XPLM300)
    windowParams.handleRightClickFunc = nullptr;  // No right-click handler needed
#endif
    
    window_id = XPLMCreateWindowEx(&windowParams);
    
    if (window_id == nullptr) {
        XPLMDebugString("ERROR: Failed to create NavigationWindow\n");
    }
}

NavigationWindow::~NavigationWindow()
{
    if (window_id != nullptr) {
        XPLMDestroyWindow(window_id);
    }
}

void NavigationWindow::setAircraftState(const AircraftState& state)
{
    current_aircraft_state = state;
}

void NavigationWindow::show()
{
    if (window_id != nullptr) {
        XPLMSetWindowIsVisible(window_id, 1);
    }
}

void NavigationWindow::hide()
{
    if (window_id != nullptr) {
        XPLMSetWindowIsVisible(window_id, 0);
    }
}

bool NavigationWindow::isVisible() const
{
    if (window_id == nullptr) {
        return false;
    }
    return XPLMGetWindowIsVisible(window_id) != 0;
}

void NavigationWindow::onDraw()
{
    if (window_id == nullptr) {
        return;
    }
    
    // Get window geometry
    int left, top, right, bottom;
    XPLMGetWindowGeometry(window_id, &left, &top, &right, &bottom);
    
    int windowWidth = right - left;
    int windowHeight = top - bottom;
    
    // Get navigation data
    NavigationDisplayData data = data_provider->getDisplayData(current_aircraft_state);
    
    // Render
    renderer->render(data, windowWidth, windowHeight);
}

// Static callback implementations
void NavigationWindow::drawCallback(XPLMWindowID inWindowID, void* inRefcon)
{
    NavigationWindow* window = static_cast<NavigationWindow*>(inRefcon);
    if (window != nullptr) {
        window->onDraw();
    }
}

int NavigationWindow::mouseClickCallback(XPLMWindowID inWindowID, int x, int y, 
                                         XPLMMouseStatus inMouse, void* inRefcon)
{
    // Return 0 to allow window to be moved/resized
    return 0;
}

XPLMCursorStatus NavigationWindow::cursorCallback(XPLMWindowID inWindowID, int x, int y, void* inRefcon)
{
    return xplm_CursorDefault;
}

int NavigationWindow::mouseWheelCallback(XPLMWindowID inWindowID, int x, int y, 
                                          int wheel, int clicks, void* inRefcon)
{
    // Don't consume wheel events
    return 0;
}

void NavigationWindow::keyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags,
                                   char inVirtualKey, void* inRefcon, int losingFocus)
{
    // Don't consume keys
}
