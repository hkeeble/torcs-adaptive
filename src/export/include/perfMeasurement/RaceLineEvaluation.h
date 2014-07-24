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

		std::vector<double> trajectoryRatings; /*!< The individual trajectory ratings collected by the evaluator. */
		std::vector<double> speedRatings; /*!< The individual speed ratings collected by the evaluator. */

		int currentSegmentID; /*!< ID of the current segment ID for which performance needs to be evaluated. */
		int currentPathSegID; /*!< ID of the current path segment. */

		const tdble speedWeight, positionWeight;

		//! Smooths the given values by a given smooth width. The higher the width, the smoother the values will be.
		/*!
			\param data The data to smooth.
			\param smoothWidth Smooth width. Higher values produce smoother curves. Must be above 0, and below size of the data set.
			\return The smoothed version of the data passed in.
		*/
		std::vector<tdble> Smooth(std::vector<tdble> data, int smoothWidth);
		
		//! Smooths all the speed values currently held by the pathfinder by the given width.
		/*!
			\param width The width of the rectangular smoothing function.
		*/
		void SmoothSpeeds(int width);

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