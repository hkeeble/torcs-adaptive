/*
    File: PTrackDescState.h
    Author: Henri Keeble
	Desc: Declares a class to hold the current state of a track description.
*/

#ifndef _P_TRACK_DESC_STATE_H_
#define _P_TRACK_DESC_STATE_H_

#include "track.h"

namespace procBot
{
	/*
		Represents the internal state of the track description for TORCS adaptive. Although the track pointed to is changing, the track description
		itself will need to be updated when new segments are added.
	*/
	class PTrackDescState
	{
	private:
		int segsOnLastUpdate;
		tTrack *torcstrack;

		// Segment that was at the end of the track on the last update call
		tTrackSeg* lastEnd;
		tTrackSeg* start; // The start segment
	public:
		PTrackDescState() : segsOnLastUpdate(0), torcstrack(nullptr), lastEnd(nullptr) { }
		bool IsUpdateNeeded();

		// Get and set accessors
		tTrack*	   GetTorcsTrack();
		tTrackSeg* GetLastEnd();
		tTrackSeg* GetStart();
		void	   SetStart(tTrackSeg* seg);
		void	   SetTorcsTrack(tTrack* track);
		void	   SetLastEnd(tTrackSeg* seg);
	};
}

#endif // _P_TRACK_DESC_STATE_H_