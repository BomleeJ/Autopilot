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

float myFlightLoop(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void * inRefcon) {
	
	
	XPLMDebugString("WE ARE INSIDE THE CALLBACK AGL: ");
	float agl = val.getDataRefValue<float>("altitude", "agl_ft");
	std::string flt = std::to_string(agl);
	XPLMDebugString(flt.c_str());
	return 1.0;
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

