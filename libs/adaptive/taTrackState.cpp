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
		totLength = wi2 = xr = yr = xl = yl = radius = alf = 0.f;
	}

	void taTrackState::ReportState()
	{
		std::cout << std::endl << " ---- TRACK STATE REPORT ----" << std::endl;
		std::cout << "Current Segment: " << curSegIndex << std::endl;
		std::cout << "Total Length: " << totLength << std::endl;
		std::cout << "Env Index: " << envIndex << std::endl;
		std::cout << "Width 2: " << wi2 << std::endl;
		std::cout << "XR: " << xr << std::endl;
		std::cout << "YR: " << yr << std::endl;
		std::cout << "XL: " << xl << std::endl;
		std::cout << "YL: " << yl << std::endl;
		std::cout << "Radius: " << radius << std::endl;
	}
}