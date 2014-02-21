/*
	File: PTrackState.h
	Author: Henri Keeble
	Desc: A class to represent the current state of a track, and to keep track of essential values needed to generate segments between different calls to AddSegment.
*/

#ifndef _P_TRACK_STATE_H_
#define _P_TRACK_STATE_H_

#include <iostream>

namespace procedural
{
	/* Used to store the current state of the track for torcs-adaptive */
	struct PTrackState
	{
		PTrackState();
		int	curSegIndex, envIndex, segsSinceLastUpdate;
		float totLength, wi2, xr, yr, xl, yl, radius, alf;
		
#ifdef _DEBUG
		void ReportState();
#endif // _DEBUG
	};
}
#endif // _P_TRACK_STATE_H_