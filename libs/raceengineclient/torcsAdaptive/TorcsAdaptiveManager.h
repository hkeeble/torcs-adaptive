/*
	File: TorcsAdaptiveManager.h
	Author: Henri Keeble
	Desc: Declares a singleton class that manages TORCS-Adaptive specific races.
*/
#ifndef _TORCS_ADAPTIVE_MANAGER_H_
#define _TORCS_ADAPTIVE_MANAGER_H_

#include "perfMeasurement\perfMeasurement.h"
#include "ProceduralTrackManager.h"

namespace torcsAdaptive
{
	enum class taRaceType
	{
		Adaptive, // Adaptive race with performance measurement
		Procedural // Procedural Race without performance measurement
	};

	class TorcsAdaptiveManager
	{
	private:
		/* Private Constructor */
		TorcsAdaptiveManager();

		/* Singleton instance  */
		static TorcsAdaptiveManager* instance;

		taRaceType raceType;
		taPerfMeasurement perfMeasurement;
		ProcecudralTrackManager pTrackManager;
	public:
		static TorcsAdaptiveManager* Get();
		virtual ~TorcsAdaptiveManager();
	};
}
#endif // _TORCS_ADAPTIVE_MANAGER_H_