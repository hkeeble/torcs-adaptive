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

		TARaceType raceType;
		taPerfMeasurement* perfMeasurement;
		PTrackManager* trackManager;
	public:
		static TAManager* Get();
		virtual ~TAManager();

		/* Return the TORCS-Adaptive Race Type */
		const TARaceType& Type() const;
		
		/* Return the performance measurement object. */
		taPerfMeasurement* PerformanceMeasurement() const;
		
		/* Initialize Adaptive Manager for a TORCS-Adaptive race type */
		void Init(tCarElt* car, tRmInfo* RaceManager, PTrack* PTrack);
		
		/* Set the current race type */
		void SetRaceType(const TARaceType& RaceType);

		/* Add a segment to the track. */
		void AddSegment(const PSeg& segment);
		
		/* Update a procedural track, managing it's cache and adding segments if neccesary. */
		void UpdateTrack();

		/* Tests whether or not the TAManager is active - if either Adaptive or Procedural Mode are active */
		bool IsActive() const;
	};
}
#endif // _TORCS_ADAPTIVE_MANAGER_H_