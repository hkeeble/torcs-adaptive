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
	/*!< A class that evaluates performance based upon the racing line. Uses a strategy pattern in order to accept different pathfinding algorithms. */
	class RaceLineEvaluation : public PMEvaluator
	{
	protected:
		virtual void Evaluate() override final;
	private:
		PPathfinder* pathfinder; /*!< The pathfinder algorithm. */

		PCarDesc* carDesc;		 /*!< Car description. */
		PTrackDesc* trackDesc;   /*!< Track description. */
		PStateManager stateMngr; /*!< Procedural track description state manager. */

	public:
		RaceLineEvaluation(PPathfinder* pathfinder);
	
		virtual void Update(tdble deltaTimeIncrement, tdble currentTime) override final;
	};
}

#endif // _RACE_LINE_EVALUATION_H_