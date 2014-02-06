/*
	File: TAManager.h
	Author: Henri Keeble
	Desc: Declares a singleton class that manages TORCS-Adaptive specific races.
*/
#ifndef _TORCS_ADAPTIVE_MANAGER_H_
#define _TORCS_ADAPTIVE_MANAGER_H_

#include "perfMeasurement\perfMeasurement.h"
#include "procedural\pTrackManager.h"
#include "procedural\PRange.h"

using namespace procedural;

namespace torcsAdaptive
{
	// Currently defined track length
	#define TA_TR_LENGTH 2000

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
		taPerfMeasurement* perfMeasurement;
		PTrackManager* trackManager;
		tRmInfo* raceManager;
		tCarElt* car;

		bool isAddingSegments; // Is track still adding segments?
		bool raceOnConsole; // Is current race on console?
	public:
		static TAManager* Get();
		virtual ~TAManager();

		/* Return the TORCS-Adaptive Race Type */
		const TARaceType& Type() const;
		
		/* Return the performance measurement object. */
		taPerfMeasurement* PerformanceMeasurement() const;
		
		/* Initializes track and graphics for a TORCS Adaptive race */
		void Init(tRmInfo* RaceManager);

		/* Initializes the procedural track manager */
		void InitTrkManager(tCarElt* car);
		
		/* Initialized performance measurement */
		void InitPerfMeasurement(tCarElt* car);

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

		/* Tests whether or not the TAManager is active - if either Adaptive or Procedural Mode are active */
		bool IsActive() const;
	};
}
#endif // _TORCS_ADAPTIVE_MANAGER_H_