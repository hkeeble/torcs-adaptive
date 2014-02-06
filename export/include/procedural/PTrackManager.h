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

			/* Manages the generation of new segments for the track */
			void ManageSegmentGeneration(float skillLevel);

			/* Generates a random segment type based on the currently set chances */
			PSegType RandomSegmentType();

			/* Internal deep copy function */
			void cpy(const PTrackManager& param);

		public:
			PTrackManager();
			PTrackManager(std::string trackName, tdble trackLength);
			PTrackManager(const PTrackManager& param);
			PTrackManager& operator=(const PTrackManager& param);
			virtual ~PTrackManager();

			/* Initialize car data to be tracked for procedural generation. */
			void Init(tCarElt* car, tRmInfo* RaceManager);

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
			PCarData CarData() const;

			/* Builds the entire track into the cache and returns the resulting track. Affects the internal track. Designed to be used to output track data at the end of a race */
			tTrack* BuildTrack();

			/* Get the current track */
			PTrack* GetTrack() const;

			/* Checks if the car is currently on the last existing segment */
			bool CarOnLastSegment();
	};
}

#endif // _PROC_TRACK_MANAGER_H_