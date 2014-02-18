/*
	File: PTrackManager.h
	Author: Henri Keeble
	Desc: Declares a singleton class to manage a procedural track.
*/
#ifndef _PROC_TRACK_MANAGER_H_
#define _PROC_TRACK_MANAGER_H_

#include <ctime>
#include "PTrack.h"
#include "PRange.h"
#include "PSSGState.h"
#include "raceman.h"

namespace procedural
{
	class PTrackManager
	{
		private:
			tRmInfo* raceManager;
			PTrack* track;
			PSegFactory* segFactory;
			CarData carData;

			/* Calculate percentage of a segment left until the end */
			tdble PercentDistanceToEnd(tdble segLength, tdble distToStart);

			/* Manages the generation of new segments for the track */
			void ManageSegmentGeneration(float skillLevel);

			/* Internal deep copy function */
			void cpy(const PTrackManager& param);

			const int MAX_DIST_FROM_END = 3; // Maximum distance car can be from the end of the track before a new segment is generated

		public:
			PTrackManager();
			PTrackManager(std::string trackName, tdble trackLength, tRmInfo* RaceManager);
			PTrackManager(const PTrackManager& param);
			PTrackManager& operator=(const PTrackManager& param);
			virtual ~PTrackManager();

			/* Initialize car data to be tracked for procedural generation. */
			void Init(tCarElt* car);

			/* Add a given segment to the track contained in the race manager passed in. */
			void AddSegment(const PSeg& segment);

			/* Update a procedural track, managing it's cache and adding segments if neccesary. Takes current skill level, leave blank for random segments. */
			void Update(float skillLevel = -1.f);

			/* Updates the graphical component of the track, updating the AC file if neccesary. */
			void UpdateGraphics();

			/* Track's current total length */
			tdble CurrentLength() const;

			/* Track's total length (desired length, not necessarily this long presently) */
			tdble TotalLength() const;

			/* Currently held car data */
			CarData GetCarData() const;

			/* Get the current track */
			PTrack* GetTrack() const;

			/* Checks if the car is currently on the last existing segment */
			bool CarOnLastSegment();
	};
}

#endif // _PROC_TRACK_MANAGER_H_