/*
	File: PTrackState.cpp
	Author: Henri Keeble
	Desc: Defines a class to represent the current state of a track, and to keep track of essential values needed to generate segments between different calls to AddSegment.
*/
#include "PTrackState.h"

namespace procedural
{
	PTrackState::PTrackState()
	{
		curSegIndex = envIndex = segsSinceLastUpdate = 0;
		totLength = wi2 = xr = yr = xl = yl = radius = alf = 0.f;
	}

#ifdef _DEBUG
	void PTrackState::ReportState()
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
#endif // _DEBUG
}