/*
	File: PTrackManager.h
	Author: Henri Keeble
	Desc: Declares a singleton class to manage a procedural track.
*/
#ifndef _PROC_TRACK_MANAGER_H_
#define _PROC_TRACK_MANAGER_H_

#include "PTrack.h"
#include "PRange.h"
#include "raceman.h"

namespace procedural
{
	class PTrackManager
	{
		private:
			tRmInfo* raceManager;
			PTrack* track;
			PSegFactory* segFactory;
			PCarData carData;

			const PSegmentRanges ranges = PSegmentRanges(PRange(MIN_LENGTH, MAX_LENGTH), PRange(MIN_ARC, MAX_ARC), PRange(MIN_RADIUS, MAX_RADIUS));

			/* Calculate percentage of a segment left until the end */
			tdble PercentDistanceToEnd(tdble segLength, tdble distToStart);

			/* Update the AC File of a track contained within the race manaegr passed in. Also requires procedural track information. */
			void UpdateACFile();

		public:
			PTrackManager();
			virtual ~PTrackManager();

			/* Initialize car data to be tracked for procedural generation. */
			void Init(tCarElt* car, tRmInfo* RaceManager, PTrack* Track);

			/* Add a given segment to the track contained in the race manager passed in. */
			void AddSegment(const PSeg& segment);

			/* Update a procedural track, managing it's cache and adding segments if neccesary. */
			void UpdateTrack();
	};
}

#endif // _PROC_TRACK_MANAGER_H_