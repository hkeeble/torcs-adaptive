/**
 * 	@file   PTrackManager.h
 *	@author Henri Keeble
 *	@brief  Declares a singleton class to manage a procedural track.
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

/*!
 * Namespace for all classes within this library.
 */
namespace procedural
{
	// Segments cars can be from the end of the track before a new segment is generated
	#define MAX_SEGS_FROM_END 4

	/*
	 *! Represents a procedural track type.
	 */
	enum PTrackType
	{
		PROCEDURAL, /*!< The track is procedural, and thus will generate new segments where neccesary. */
		PREGENERATED /*< The track is a pregenerated one, and will therefore not generate any new segments. */
	};

	/*!
	 * This class manages the lifetime of a procedural track, and abstracts it's functionality away from client code.
	*/
	class PTrackManager
	{
		private:
			tRmInfo* raceManager;		 /*!< Pointer to the current race manager. */
			PTrack* track;				 /*!< Pointer to the current procedural track being managed. */
			PSegFactory* segFactory;	 /*!< Pointer to the segment factory. */
			tCarElt* carList;			 /*!< Pointer the car list. This is used in segment generation. */
			int nCars;					 /*!< The number of cars in the current race that need to be monitored. */

			int previousSegType;			/*!< The type of previous segment generated. */
			PCornerType previousCornerType;	/*!< The previous corner type that was generated. */

			PTrackType trackType; /*!< The current type of track being managed. */

			//! Generates a new segment for the track based upon a skill level.
			/*! 
				\param skillLevel The player's currently estimated skill level. Must be between 0.0 and 1.0, inclusive.
			 */
			void GenerateAdaptiveSegment(tdble skillLevel);

			/* Internal deep copy function */
			void cpy(const PTrackManager& param);

			/*! Update the current track's AC file */
			void UpdateACFile();

		public:

			/*
			 * Default constructor initialization. Sets all pointers to null. Other initialization functions need to be called before
			 * the manager is ready for use. The constructor overload should usually be used before these functions, in order to give
			 * the manager a pointer to the race manager being used by the current instance of TORCS, as this pointer is used in a
			 * number of functions within the class.
			 */
			PTrackManager();
			 
			//! Initializes a new procedural track manager.
			/*!
				\param RaceManager Pointer to the current race manager object being used by TORCS.
			*/
			PTrackManager(tRmInfo* RaceManager);

			PTrackManager(const PTrackManager& param);
			PTrackManager& operator=(const PTrackManager& param);
			virtual ~PTrackManager();

			/**
			 * Initialize a procedural track. This initialization uses all data currently set in the PFileManager load state.
			*/
			void InitTrack();

			//! Add a given segment to the track. This function does not handle creation of a 3D description file.
			//!	segment The segment to append to the track.
			/*!
				\param segment The segment to add to the track. This can be generated using the PSegFactory.
			 */
			void AddSegment(const PSeg& segment);

			//! Update a procedural track, updating the car data held and the track itself. This function will add new segments if neccesary.
			/*!
				\param adaptive Whether or not track is adaptive and making use of a skill level when adding segments. Defaults to false.
				\param skillLevel The skill level currently recorded for the player. Interpolates between values, should be between 1.0 and 0.0.
			 */
			void Update(bool adaptive = false, float skillLevel = -1.f);

			/** Returns a pointer to the car currently in the lead of the race. */
			tCarElt* LeadingCar() const;

			/** Updates the graphical component of the track, updating the AC file if neccesary. */
			void UpdateGraphics();

			/** Track's current total length */
			tdble CurrentLength() const;

			/** Track's total length (desired length, not necessarily this long presently) */
			tdble TotalLength() const;

			/** Get the current track */
			PTrack* GetTrack() const;

			/** Checks if any of the cars in the race have reached the maximum race distance */
			bool IsCarFinished();

			/** This functions is used in order to prevent cars leaving the back of the track (a cleaner solution for this needs to be found) */
			void CorrectCars();

			//! Outputs the current track
			/*!
				\param name The name to output the track under. This will dictate the construction of all the file names output.
			 */
			void OutputCurrentTrack(std::string name);

			/** Initializes the cars for the procedural race */
			void InitCars();
	};
}

#endif // _PROC_TRACK_MANAGER_H_