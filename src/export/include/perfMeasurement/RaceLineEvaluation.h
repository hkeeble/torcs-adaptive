/**
*	@file RaceLineEvaluation.h
*	@author Henri Keeble
*	@desc 	Declares a class that evaluates performance based upon the racing line. Uses a strategy pattern in order to accept different pathfinding algorithms.
*/

#ifndef _RACE_LINE_EVALUATION_H_
#define _RACE_LINE_EVALUATION_H_

#include "PMEvaluator.h"
#include "PPathfinder.h"

// Procedural pathfinder namespace
using namespace procPathfinder;

namespace perfMeasurement
{
	enum class Pathfinder
	{
		K1999,
		Berniw
	};

	//! A class that evaluates performance based upon the racing line. Uses a strategy pattern in order to accept different pathfinding algorithms.
	class RaceLineEvaluation : public PMEvaluator
	{
	protected:
		virtual void Evaluate() override final;
	private:
		PPathfinder* pathfinder; /*!< The pathfinder algorithm. */

		PCarDesc* carDesc;		 /*!< Car description. */
		PTrackDesc* trackDesc;   /*!< Track description. */
		PStateManager stateMngr; /*!< Procedural track description state manager. */

		std::vector<PMData> actualData; /*!< Represents all recorded actual data for the car. Stored such that the evaluator can release data
											 used for actual performance measurement after each evaluation. */

		int currentSegmentID; /*!< ID of the current segment ID for which performance needs to be evaluated. */
		int currentPathSegID; /*!< ID of the current path segment. */

	public:
		//! Create a new race-line evaluation object.
		/*!
			\param car		  The car to evaluate.
			\param pathfinder The pathfinder algorithm to use.
			\param track	  The track being evaluated.
			\param situation  Situation object from the race manager.
		*/
		RaceLineEvaluation(tCarElt* car, Pathfinder pathfinder, tTrack* track, tSituation* situation);
		
		//! Used to calculate the optimal points the optimal arc must pass through.
		/*!
			\param deltaIncrement delta time.
			\param currentTime The current time elapsed.
		*/
		virtual void Update(tdble deltaTimeIncrement, tdble currentTime) override final;

		virtual void RaceEnd() override final;
	};
}

#endif // _RACE_LINE_EVALUATION_H_