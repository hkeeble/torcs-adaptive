/**
 *	@file PMEvaluator.h
 *
 *	@author Henri Keeble
 *	@desc 	Declarations for a base class for all evaluators.
*/

#ifndef _PM_EVALUATOR_H_
#define _PM_EVALUATOR_H_

#include <vector>

#include "PMData.h"
#include "PMDefs.h"
#include "taMath\taMath.h"
#include "robottools.h"

namespace perfMeasurement
{
	// Unit conversion macros \\
	
	// Feet Per Second to KMPH
	#define FPS_TO_KMPH(F) F*1.09728
	
	// Gees to Meters Per Second
	#define G_TO_MPS(G) G*9.80665

	/*!
	 * Base Evaluation class. The idea is to derive from this class with bespoke update and
	 * evaluation behaviours, such that a data set is maintained and evaluated correctly for each method
	*/
	class PMEvaluator
	{
	protected:
		tCarElt* car;				 /*!< The car being evauluated by this evaluator. */
		std::vector<PMData> dataSet; /*!< The current data set */
		std::vector<tdble> estimates;/*!< Set of all the skill estimates for this race. */
		tdble currentEstimate;		 /*!< The current estimate for skill level */
		tdble totalEstimate;		 /*!< The total skill level estimate (appended to each time an evaluation is made) */
		tdble avgEstimate;			 /*!< The average estimate (totalEstimate/nEstimates) */
		int nEstimates;				 /*!< The number of estimates that have been made in this race */
		
		virtual void Evaluate() = 0; /*!< Pure virtual member function. Override this for bespoke evaluation functionality. */
	public:
	
		/** Initializes a new evaluator, initializing the skill estimate to a null skill level. */
		PMEvaluator(tCarElt* car) : currentEstimate(NULL_SKILL_LEVEL), totalEstimate(0), avgEstimate(0), nEstimates(0), dataSet(std::vector<PMData>()),
			estimates(std::vector<tdble>()), car(car) { }
		
		/** Pure virtual update function. Override for bespoke update functionality, is also responsible for calling the Evaluate function. */
		virtual void Update(tdble deltaTimeIncrement, tdble currentTime) = 0;
		
		/** Called at the end of each race. */
		virtual void RaceEnd() = 0;

		/** Get the car pointed to by the evaluator. */
		tCarElt* GetCar() { return car; }
		
		/** Clear data held by the evaluator. */
		void ClearData() { dataSet.clear(); }
		
		/** Retrieve the current skill level estimate from the evaluator. */
		tdble GetCurrentEstimate() const { return currentEstimate; }
		
		/** Retrieve the current average skill level estimate from the evaluator. */
		tdble GetAverageEstimate() const { tdble sum = 0; for (auto i : estimates) sum += i; return sum / estimates.size(); }

		/** Retrieve a collection of all the estimates in sequential order for the current race. */
		const std::vector<tdble> GetEstimates() const { return estimates; }
	};
}

#endif //_PM_EVALUATOR_H_