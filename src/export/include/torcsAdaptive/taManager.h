/*
	File: TAManager.h
	Author: Henri Keeble
	Desc: Declares a singleton class that manages TORCS-Adaptive specific races.
*/
#ifndef _TORCS_ADAPTIVE_MANAGER_H_
#define _TORCS_ADAPTIVE_MANAGER_H_

#include <windows.h>
#include <sstream>

#include "perfMeasurement\PMManager.h"
#include "procedural\pTrackManager.h"
#include "procedural\PFileManager.h"
#include "procedural\PMenu.h"
#include "procedural\PRange.h"
#include "TAHud.h"

// Using both TORCS-Adaptive namespace features. These are both seperate libaries and namespaces and are fully independent of one another.
using namespace procedural;
using namespace perfMeasurement;

// TORCS-Adaptive specific debug output - used for debug information output from the TAManager, not procedural or perfMeasurement
#ifdef _DEBUG
	#define taOut(out) std::cout << "torcs-adaptive >>" + std::string(out)
#else
	#define taOut(out)
#endif // _DEBUG

namespace torcsAdaptive
{
	// Currently defined track length
	#define TA_TR_LENGTH 100

	// Race Manager Names
	#define ADAPTIVE_RACE "Adaptive Race"
	#define PROCEDURAL_RACE "Procedural Race"

	/* Type of TORCS-Adaptive Race currently active.
		0 - None: Assigned as default, therefore only set if TAManager::Init() has not been called, adaptive/procedural race not active.
		1 - Adaptive: Adaptive race with performance measurement.
		2 - Procedural: Procedural Race without performance measurement.
		3 - Pregenerated: A race with a previously procedurally generated track is loaded.
	*/
	enum class TARaceType
	{
		None,
		Adaptive,
		Procedural,
		Pregenerated
	};

	class TAManager
	{
	private:
		/* Private Constructor */
		TAManager();

		/* Singleton instance  */
		static TAManager* instance;

		// The current player's skill level
		float currentSkillLevel;

		// The current race type
		TARaceType raceType;

		// Pointers to main management objects
		PMManager* perfMeasurement;
		PTrackManager* trackManager;
		PFileManager* fileManager;
		tRmInfo* raceManager;

		// Pointer to the car
		tCarElt* car;

		// Is current race on console?
		bool raceOnConsole;

		// The TORCS Adaptive HUD
		TAHud hud;

		/* Output the current track */
		void OutputTrack();

	public:
		static TAManager* Get();
		virtual ~TAManager();

		/* Return the TORCS-Adaptive Race Type */
		const TARaceType& Type() const;
		
		/* Return the performance measurement object. */
		PMManager* PerformanceMeasurement() const;
		
		/* Initializes track and graphics for a TORCS Adaptive race */
		void Init(tRmInfo* RaceManager);
		
		/* Initialize performance measurement. Pass in the car to monitor, and an evaluation behaviour to use. */
		void InitPerfMeasurement(tCarElt* car, PMEvaluator* evaluator);

		/* Initialize Graphics, must call Init and InitTrack before this */
		void InitGraphics();

		/* Initialize track, must call Init before this */
		void InitTrack();

		/* Set the current race type */
		void SetRaceType(const TARaceType& RaceType);

		/* Get the current race type */
		TARaceType GetRaceType() const;

		/* Get the current track */
		PTrack* GetTrack() const;

		/* Get the track manager */
		PTrackManager* GetTrackManager() const;

		/* Add a segment to the track. */
		void AddSegment(const PSeg& segment);
		
		/* Update a procedural track, managing it's cache and adding segments if neccesary. */
		void UpdateTrack();

		/* Check if race is finished, car has traversed entire distance of track. */
		void CheckIfFinished();

		/* Call at end of race */
		void RaceEnd();

		/* Initializes the position of the car on a TORCS Adaptive track */
		void InitCars();

		/* Tests whether or not the TAManager is active - if either Adaptive or Procedural Mode are active */
		bool IsActive() const;

		/* Tests whether or not the TAManager is in a procedural race mode - that is, if a pregenerated track is not loaded */
		bool IsProcedural() const;

		/* Draws TORCS-Adaptive specific content to the UI */
		void DrawBoard();
	};
}
#endif // _TORCS_ADAPTIVE_MANAGER_H_