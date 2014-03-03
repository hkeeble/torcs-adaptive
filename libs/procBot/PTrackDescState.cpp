/*
	File: PTrackDescState.h
	Author: Henri Keeble
	Desc: Defines a class to hold the current state of a track description.
*/

#include "PTrackDescState.h"

namespace procBot
{
	bool PTrackDescState::IsUpdateNeeded()
	{
		if (segsOnLastUpdate < torcstrack->nseg)
		{
			segsOnLastUpdate = torcstrack->nseg;
			return true;
		}
		else
			return false;
	}

	tTrack* PTrackDescState::GetTorcsTrack()
	{
		return torcstrack;
	}

	void PTrackDescState::SetTorcsTrack(tTrack* track)
	{
		torcstrack = track;
	}

	void PTrackDescState::SetLastEnd(tTrackSeg* seg)
	{
		lastEnd = seg;
	}

	tTrackSeg* PTrackDescState::GetLastEnd()
	{
		return lastEnd;
	}

	tTrackSeg* PTrackDescState::GetStart()
	{
		return start;
	}

	void PTrackDescState::SetStart(tTrackSeg* seg)
	{
		this->start = seg;
	}
}