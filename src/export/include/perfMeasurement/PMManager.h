/**
 *	@file PMManager.h
 *	@author Henri Keeble
 *	@brief Declaration for performance measurement management class.
*/

#ifndef _PM_MANAGER_H_
#define _PM_MANAGER_H_

#include <fstream>
#include <vector>

#include "PMData.h"
#include "PMEvaluator.h"
#include "RaceLineEvaluation.h"
#include "PMDefs.h"
#include "car.h"

namespace perfMeasurement
{
	/*! Class used as interface to performance measurement in a game session. */
	class PMManager
	{
	private:
		PMManager() { };
		PMManager(const PMManager& param) { };
		PMManager& operator=(const PMManager& param) { };

		static PMManager* instance;	 /*!< The current singleton instance of PMManager */
		
		PMEvaluator* Evaluator; /*!< Pointer to object containing the currently in use evaluation behaviour */

	public:
		static PMManager* Get();
		~PMManager();
		
		//! Initialization function.
		 /*!
			\param car		 The car to monitor.
			\param evaluator An uninitialized evaluator to use.
		 */
		void Init(tCarElt* car, PMEvaluator* evaluator);

		/** Get the car currently being monitored */
		tCarElt* GetCar();

		//!Update the current evaluator.
		 /*!
			\param deltaTimeIncrement The time since the last invocation of this function.
			\param currentTime		  The current time stamp to use when recording data.
		 */
		void Update(tdble deltaTimeIncrement, tdble currentTime);
		
		/** Clear the current collection of performance measurement data */
		void Clear();
		
		/** Retrieve the current estimate skill level */
		tdble GetSkillEstimate();

		/** Retrieve the current average skill estimate from the evaluator. */
		tdble GetAverageEstimate();

		/** Output the performance measurement data for the current race. */
		void OutputData(std::string filepath);
	};
}
#endif // _PM_MANAGER_H_