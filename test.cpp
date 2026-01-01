// Downloaded from https://developer.x-plane.com/code-sample/hello-world-sdk-3/
#define APL 1
#define XPLM300 1
#define XPLM301 1

#include "XPLMPlugin.h"
#include "XPLMDefs.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMUtilities.h"
#include "XPLMProcessing.h"
#include "AircraftIO.h"
#include "FlightState.h"
#include "PID.h"
#include <stdio.h>
#include <string.h>
#if IBM
	#include <windows.h>
#endif
#if LIN
	#include <GL/gl.h>
#elif __GNUC__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#ifndef XPLM300
	#error This is made to be compiled against the XPLM300 SDK
#endif

// An opaque handle to the window we will create


// FlightLoop callbacks
XPLM_API XPLMFlightLoopID XPLMCreateFlightLoop(XPLMCreateFlightLoop_t * inParams);


XPLMFlightLoopID flightLoopID;
AircraftIO val = AircraftIO("datarefs.json");
FlightStateManager flightStateManager = FlightStateManager();
NavigationManager navigationManager = NavigationManager("navigation.json");
ThrottlePIDController throttlePIDController = ThrottlePIDController(0.8f, 0.4f, 0.1f);
float myFlightLoop(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void * inRefcon) {
	AircraftState aircraft_state = val.getAircraftState();

	if (aircraft_state.position.altitude_agl_ft < 100.0f) {
		return -1;
	}
	std::optional<AircraftGuidance> guidance = flightStateManager.getGuidance(aircraft_state, navigationManager);
	if (!guidance.has_value()) {
		return 1.0f;
	}
	AircraftGuidance guidance_value = guidance.value();
	float SpeedError = throttlePIDController.calculateError(guidance_value.kinematics_targets.indicated_airspeed_knots, aircraft_state.kinematics.indicated_airspeed_knots);
	float throttle_output = throttlePIDController.calculate(SpeedError);
	val.setThrottlePosition(throttle_output);
	XPLMDebugString("Throttle output:");
	XPLMDebugString(std::to_string(throttle_output).c_str());
	XPLMDebugString("\n");
	return -1.0f;
	
}

void createFlightLoop() {
	XPLMFlightLoop_f flightLoopCallback = myFlightLoop;
	XPLMCreateFlightLoop_t flightLoopParams;
	XPLMDebugString("WE ARE CREATING THE FLIGHT LOOP\n");
	flightLoopParams.structSize = sizeof(XPLMCreateFlightLoop_t);
	flightLoopParams.phase = 0;
	flightLoopParams.callbackFunc = flightLoopCallback;
	flightLoopParams.refcon = NULL;
	flightLoopID = XPLMCreateFlightLoop(&flightLoopParams);
	XPLMScheduleFlightLoop(flightLoopID, 1.0, 0);
}
PLUGIN_API int XPluginStart(
							char *		outName,
							char *		outSig,
							char *		outDesc)
{
	
	strcpy(outName, "C172 Autopilot");
	strcpy(outSig, "xpsdk.autopilot.c172");
	strcpy(outDesc, "An experimental autopilot system plugin for the Cessna 172, built with XPLM300 SDK.");
	
	createFlightLoop();
	return 1;
	
}

PLUGIN_API void	XPluginStop(void)
{	
	
	XPLMDestroyFlightLoop(flightLoopID);
	XPLMDebugString("WE HAVE STOPPED THE PLUGIN\n");
	return;
}

PLUGIN_API void XPluginDisable(void) { }
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam) { }

