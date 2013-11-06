/*
	File: torcsAdaptive.h
	Desc: header file for torcsAdaptive
	Author: Henri Keeble
	Created: 05/11/2013
	Last Edit: 06/11/2013
*/
#ifndef _TORCS_ADAPTIVE_
#define _TORCS_ADAPTIVE_

#include "raceman.h"
#include "perfMeasurement\perfMeasurement.h"
#include "seg.h"
#include <string>

namespace torcsAdaptive
{
	#define ADAPTIVE_MAN_NAME "Adaptive Race" // Name of adaptive race manager
	#define LENGTH_PER_SEG 40 // Length in meters of each segment

	extern bool taAdaptiveMode;
	extern taPerfMeasurement* perfMeasurement;

	// TA Functions
	int TaInitTrack(tRmInfo* ReInfo, int trkLength);
	void TaShutDown();
	void TaAddSegment(taSeg seg);

	
}

#endif // _TORCS_ADAPTIVE_