/*
	File: PMManager.h
	Author: Henri Keeble
	Desc: Declaration for performance measurement management class.
*/

#ifndef _PM_MANAGER_H_
#define _PM_MANAGER_H_

#include "PMData.h"
#include "PMEvaluator.h"
#include "RaceLineEvaluation.h"
#include "PMDefs.h"
#include "car.h"

namespace perfMeasurement
{
	/* Class used as interface to performance measurement in a game session. */
	class PMManager
	{
	private:
		/* Private construction - uncopyable singleton */
		PMManager() { };
		PMManager(const PMManager& param) { };
		PMManager& operator=(const PMManager& param) { };

		/* The current singleton instance of PMManager */
		static PMManager* instance;

		/* Pointer to object containing the currently in use evaluation behaviour */
		PMEvaluator* Evaluator;

	public:
		static PMManager* Get();
		~PMManager();
		
		/* 
		 * Initialization function.
		 * car		 The car to monitor.
		 * evaluator An uninitialized evaluator to use.
		 */
		void Init(tCarElt* car, PMEvaluator* evaluator);

		/* Get the car currently being monitored */
		tCarElt* GetCar();
		
		/*
		 * Update the current evaluator.
		 * deltaTimeIncrement The time since the last invocation of this function.
		 * currentTime		  The current time stamp to use when recording data.
		 */
		void Update(tdble deltaTimeIncrement, tdble currentTime);
		
		/* Clear the current collection of performance measurement data */
		void Clear();
		
		/* Retrieve the current estimate skill level */
		tdble GetSkillEstimate();
	};
}
#endif // _PM_MANAGER_H_