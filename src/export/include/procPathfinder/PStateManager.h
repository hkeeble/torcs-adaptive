/*
    File: PTrackDescState.h
    Author: Henri Keeble
	Desc: Declares a class to hold and update the current state of a track.
*/

#ifndef _P_TRACK_DESC_STATE_H_
#define _P_TRACK_DESC_STATE_H_

#include "track.h"

namespace procPathfinder
{
	/*
		Represents the internal state of a track, can be used by any object that must update according to an existing track.
	*/
	class PStateManager
	{
	private:
		int currentSegs; // Current number of segments in the track
		int segsOnLastUpdate; // Segments in the track on the last call to update

		// Pointer to the TORCS track
		tTrack *torcstrack;

		// Whether or not an update is currently required by the parent object
		bool updateNeeded;

		// Segment that was at the end of the track on the last update call
		tTrackSeg* lastEnd;

		// The start segment
		tTrackSeg* trackStart;

		// The end segment
		tTrackSeg* trackEnd;

		/* Initialize the state when given a new track, internal use only */
		void Init();
	public:
		/* Constructors */
		PStateManager() : segsOnLastUpdate(0), torcstrack(nullptr), lastEnd(nullptr), currentSegs(0), updateNeeded(false) {}
		
		/* Constructs a state manager with a track pointer, and initializes the start and end pointers. */
		PStateManager(tTrack* const track);

		/* Updates the state manager */
		void Update();

		/* Returns whether or not an update is currently needed */
		bool IsUpdateNeeded();

		/* Get pointer to the TORCS track structure */
		tTrack* GetTorcsTrack();
		
		/* Get pointer to the segment at the end of the TORCS track structure */
		tTrackSeg* GetLastEnd();
		
		/* Get pointer to the segment at the start of the TORCS track structure */
		tTrackSeg* GetStart();
		
		/* Get pointer to the segment at the end of the TORCS track structure */
		tTrackSeg* GetEnd();

		/* Get the TORCS track */
		void SetTorcsTrack(tTrack* const track);
	};
}

#endif // _P_TRACK_DESC_STATE_H_