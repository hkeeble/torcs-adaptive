/*
	File: taTrackState.cpp
	Desc: definitions file for torcsAdaptive track state class
	Author: Henri Keeble
*/
#include "torcsAdaptive\taTrackState.h"

namespace torcsAdaptive
{
	taTrackState::taTrackState()
	{
		curSegIndex = envIndex = segsSinceLastUpdate = 0;
		totLength = wi2 = xr = yr = xl = yl = radius = 0.f;
	}
}