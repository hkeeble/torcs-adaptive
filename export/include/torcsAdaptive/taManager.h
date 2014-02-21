/*
	File: TAManager.h
	Author: Henri Keeble
	Desc: Declares a singleton class that manages TORCS-Adaptive specific races.
*/
#ifndef _TORCS_ADAPTIVE_MANAGER_H_
#define _TORCS_ADAPTIVE_MANAGER_H_

#include "perfMeasurement\PMManager.h"
#include "procedural\pTrackManager.h"
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
	#define TA_TR_LENGTH 5000

	/* Type of TORCS-Adaptive Race currently active.
		0 - None: Assigned as default, therefore only set if TAManager::Init() has not been called, adaptive/procedural race not active.
		1 - Adaptive: Adaptive race with performance measurement.
		2 - Procedural: Procedural Race without performance measurement.
	*/
	enum class TARaceType
	{
		None,
		Adaptive,
		Procedural
	};

	class TAManager
	{
	private:
		/* Private Constructor */
		TAManager();

		/* Singleton instance  */
		static TAManager* instance;

		float currentSkillLevel;
		TARaceType raceType;
		PMManager* perfMeasurement;
		PTrackManager* trackManager;
		tRmInfo* raceManager;
		tCarElt* car;

		bool raceOnConsole; // Is current race on console?

		// The TORCS Adaptive HUD
		TAHud hud;
	
	public:
		static TAManager* Get();
		virtual ~TAManager();

		/* Return the TORCS-Adaptive Race Type */
		const TARaceType& Type() const;
		
		/* Return the performance measurement object. */
		PMManager* PerformanceMeasurement() const;
		
		/* Initializes track and graphics for a TORCS Adaptive race */
		void Init(tRmInfo* RaceManager);

		/* Initializes the procedural track manager */
		void InitTrkManager(tCarElt* car);
		
		/* Initialize performance measurement. Pass in the car to monitor, and an evaluation behaviour to use. */
		void InitPerfMeasurement(tCarElt* car, PMEvaluator* evaluator);

		/* Initialize Graphics, must call Init and InitTrack before this */
		void InitGraphics();

		/* Initialize track, must call Init before this */
		void InitTrack(std::string trackName);

		/* Set the current race type */
		void SetRaceType(const TARaceType& RaceType);

		/* Get the current race type */
		TARaceType GetRaceType() const;

		/* Get the current track */
		PTrack* GetTrack() const;

		/* Add a segment to the track. */
		void AddSegment(const PSeg& segment);
		
		/* Update a procedural track, managing it's cache and adding segments if neccesary. */
		void UpdateTrack();

		/* Check if race is finished, car has traversed entire distance of track. */
		void CheckIfFinished();

		/* Call at end of race */
		void RaceEnd();

		/* Initializes the position of the car on a TORCS Adaptive track */
		void InitCarPos();

		/* Tests whether or not the TAManager is active - if either Adaptive or Procedural Mode are active */
		bool IsActive() const;

		/* Draws TORCS-Adaptive specific content to the UI */
		void DrawBoard();
	};
}
#endif // _TORCS_ADAPTIVE_MANAGER_H_