/*
	File: torcsAdaptive.h
	Desc: header file for torcsAdaptive
	Author: Henri Keeble
	Created: 05/11/2013
	Last Edit: 05/11/2013
*/
#ifndef _TORCS_ADAPTIVE_
#define _TORCS_ADAPTIVE_

#include "raceman.h"

namespace torcsAdaptive
{
	#define ADAPTIVE_MAN_NAME "Adaptive Race" // Name of adaptive race manager

	extern bool taAdaptiveMode;

	int TaInitTrack(tRmInfo* ReInfo);
}

#endif // _TORCS_ADAPTIVE_