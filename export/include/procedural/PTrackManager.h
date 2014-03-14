/*
	File: PTrackManager.h
	Author: Henri Keeble
	Desc: Declares a singleton class to manage a procedural track.
*/
#ifndef _PROC_TRACK_MANAGER_H_
#define _PROC_TRACK_MANAGER_H_

#include <ctime>
#include <fstream>
#include <vector>
#include <sstream>

#include "PTrack.h"
#include "PRange.h"
#include "PSSGState.h"
#include "raceman.h"
#include "PTrackLoadState.h"

namespace procedural
{
	/*
	 * Represents a procedural track type.
	 * PROCEDURAL	The track is procedural, and thus will generate new segments where neccesary.
	 * PREGENERATED The track is a pregenerated one, and will therefore not generate any new segments.
	 */
	enum PTrackType
	{
		PROCEDURAL,
		PREGENERATED
	};

	/*
	 * A procedural track manager object. This class manages the lifetime of a procedural track, and abstracts it's functionality away from client code.
	*/
	class PTrackManager
	{
		private:
			tRmInfo* raceManager;	 /* Pointer to the current race manager. */
			PTrack* track;			 /* Pointer to the current procedural track being managed. */
			PSegFactory* segFactory; /* Pointer to the segment factory. */
			CarData carData;		 /* The current car data. */
			int previousSegType;	 /* The type of previous segment generated. */

			PTrackType trackType; /* The current type of track being managed. */

			/*
			 * Generates a new segment for the track based upon a skill level.
			 * skillLevel The player's currently estimated skill level. Must be between 0.0 and 1.0, inclusive.
			 */
			void GenerateAdaptiveSegment(tdble skillLevel);

			/* Internal deep copy function */
			void cpy(const PTrackManager& param);

			/* Update the current track's AC file */
			void UpdateACFile();

			const int MAX_DIST_FROM_END; // Maximum distance car can be from the end of the track before a new segment is generated

		public:
			PTrackManager();

			/* 
			 * Initializes a new procedural track manager.
			 * RaceManager Pointer to the current race manager object.
			*/
			PTrackManager(tRmInfo* RaceManager);

			PTrackManager(const PTrackManager& param);
			PTrackManager& operator=(const PTrackManager& param);
			virtual ~PTrackManager();

			/*
			 * Initialize car data to be tracked for procedural generation.
			 * car The car to be tracked. This is required as the position of this car needs to be known when generating new segments.
			 */
			void Init(tCarElt* car);

			/*
			 * Initialize a procedural track given a configuration
			 * loadState The current load state. Tells the trackmanager what type of track to load - a pregenerated one or a configuration, and also where to find all the neccesary files.
			*/
			void InitTrack(PTrackLoadState loadState);

			/*
			 * Add a given segment to the track. This function does not handle creation of a 3D description file.
			 * segment The segment to append to the track.
			 */
			void AddSegment(const PSeg& segment);

			/*
			 * Update a procedural track, updating the car data held and the track itself. This function will add new segments if neccesary.
			 * adaptive Whether or not the current race is adpative, and segment generation is sensitive to player skill level. Defaults to false.
			 * skillLevel The currently recorded skill level of the player. This must be between 0.0 and 1.0, inclusively. Defautls to -1.0.
			*/
			void Update(bool adaptive = false, float skillLevel = -1.f);

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

			/* Outputs the current track */
			void OutputCurrentTrack();
	};
}

#endif // _PROC_TRACK_MANAGER_H_