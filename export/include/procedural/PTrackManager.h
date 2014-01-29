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

			bool updateAC; // Determines whether AC file needs updating or not
			bool generateSegment; // Determines if the track needs a new segment generated

			const float cornerChance = 45.f;
			const float straightChance = 65.f;

			/* Generates a random float value between a given minimum and maximum, inclusive */
			float RandBetween(float min, float max);

			/* The previous corner type */
			PCornerType previousCornerType;

			/* The arbitrary ranges between which segment parameters may reside */
			const PSegmentRanges ranges = PSegmentRanges(PRange(MIN_LENGTH, MAX_LENGTH), PRange(MIN_ARC, MAX_ARC), PRange(MIN_RADIUS, MAX_RADIUS));

			/* Calculate percentage of a segment left until the end */
			tdble PercentDistanceToEnd(tdble segLength, tdble distToStart);

			/* Update the AC File of a track contained within the race manaegr passed in. Also requires procedural track information. */
			void UpdateACFile();

			/* Manages the track cache, removing segments where neccesary. */
			void ManageCache();

			/* Manages the track's AC file, updating where neccesary */
			void ManageACFile();

			/* Manages the generation of new segments for the track */
			void ManageSegmentGeneration(float skillLevel);

			/* Generates a random segment type based on the currently set chances */
			PSegType RandomSegmentType();

		public:
			PTrackManager();
			virtual ~PTrackManager();

			/* Initialize car data to be tracked for procedural generation. */
			void Init(tCarElt* car, tRmInfo* RaceManager, PTrack* Track);

			/* Add a given segment to the track contained in the race manager passed in. */
			void AddSegment(const PSeg& segment);

			/* Update a procedural track, managing it's cache and adding segments if neccesary. Takes current skill level, leave blank for random segments. */
			void Update(float skillLevel = -1.f);
	};
}

#endif // _PROC_TRACK_MANAGER_H_