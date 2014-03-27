/**
 *	@file PMEvaluator.h
 *
 *	@author Henri Keeble
 *	@desc 	Declarations for a base class for all evaluators.
*/

#ifndef _PM_EVALUATOR_H_
#define _PM_EVALUATOR_H_

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
		CarData car;				 /*!< The car data currently held. This should be updated on each call to Update. */
		std::vector<PMData> dataSet; /*!< The current data set */
		tdble currentEstimate;		 /*!< The current estimate for skill level */
		tdble totalEstimate;		 /*!< The total skill level estimate (appended to each time an evaluation is made) */
		tdble avgEstimate;			 /*!< The average estimate (totalEstimate/nEstimates) */
		int nEstimates;				 /*!< The number of estimates that have been made in this race */
		
		virtual void Evaluate() = 0; /*!< Pure virtual member function. Override this for bespoke evaluation functionality. */
	public:
	
		/** Initializes a new evaluator, initializing the skill estimate to a null skill level. */
		PMEvaluator() : currentEstimate(NULL_SKILL_LEVEL) { }
		
		/** Pure virtual update function. Override for bespoke update functionality, is also responsible for calling the Evaluate function. */
		virtual void Update(tdble deltaTimeIncrement, tdble currentTime) = 0;
		
		/** Get the car pointed to by the evaluator. */
		tCarElt* GetCar() { return car.GetCar(); }
		
		/** Clear data held by the evaluator. */
		void ClearData() { dataSet.clear(); }
		
		//! Initialize the evaluator with a car pointer.
		/*!
			\param carElt Pointer to the car structure that is controlled by the player or robot being evaluated.
		*/
		void Init(tCarElt* carElt) { car = carElt; dataSet = std::vector<PMData>(); }
		
		/** Retrieve the current skill level estimate from the evaluator. */
		tdble GetCurrentEstimate() const { return currentEstimate; }
		
		/** Retrieve the current average skill level estimate from the evaluator. */
		tdble GetAverageEstimate() const { return avgEstimate; }
	};
}

#endif //_PM_EVALUATOR_H_