/*
	File: PStateManager.h
	Author: Henri Keeble
	Desc: Defines a class to hold the current state of a track description.
*/

#include "PStateManager.h"

namespace procPathfinder
{
	PStateManager::PStateManager(tTrack* const track)
	{
		segsOnLastUpdate = 0;
		lastEnd = nullptr;
		currentSegs = 0;
		updateNeeded = false;
		torcstrack = track;
		Init();
	}

	void PStateManager::Init()
	{
		trackEnd = torcstrack->seg; // Get initial end segment
		
		tTrackSeg* curSeg = trackEnd; // Start at end, ready to search for start
		do {
			curSeg = curSeg->prev;
		} while (curSeg->prev != trackEnd);

		trackStart = curSeg; // Assign start segment

		currentSegs = torcstrack->nseg;
	}

	bool PStateManager::IsUpdateNeeded()
	{
		return updateNeeded;
	}

	void PStateManager::Update()
	{
		if (torcstrack)
		{
			currentSegs = torcstrack->nseg;
			if (currentSegs > segsOnLastUpdate)
			{
				updateNeeded = true;
				segsOnLastUpdate = currentSegs;

				// Segment pointer used for searching for start/end
				tTrackSeg* curSeg;

				curSeg = torcstrack->seg; // Obtain the current end

				if (currentSegs == 1)
					lastEnd = nullptr;
				else
					lastEnd = trackEnd;

				trackEnd = curSeg; // Assign the current end
			}
			else
				updateNeeded = false;
		}
		else
			std::cout << "Error: failed to update a PStateManager - no TORCS track pointer has been set." << std::endl;
	}

	tTrack* PStateManager::GetTorcsTrack()
	{
		return torcstrack;
	}

	void PStateManager::SetTorcsTrack(tTrack* const track)
	{
		torcstrack = track;
		Init();
	}

	tTrackSeg* PStateManager::GetLastEnd()
	{
		return lastEnd;
	}

	tTrackSeg* PStateManager::GetStart()
	{
		return trackStart;
	}

	tTrackSeg* PStateManager::GetEnd()
	{
		return trackEnd;
	}
}