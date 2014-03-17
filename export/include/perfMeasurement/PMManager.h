/*
	File: PMManager.h
	Author: Henri Keeble
	Desc: Declaration for performance measurement management class.
*/

#ifndef _PM_MANAGER_H_
#define _PM_MANAGER_H_

#include "PMData.h"
#include "PMEvaluators.h"
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
		
		/* Initialization function. Pass in a pointer to the car the manager is intended to monitor. Also pass in an unintialized evaluation object to make use of. */
		void Init(tCarElt* car, PMEvaluator* evaluator);

		/* Get the car currently being monitored */
		tCarElt* GetCar();
		
		/* Update the performance measurement data */
		void Update(tdble deltaTimeIncrement, tdble currentTime);
		
		/* Clear the current collection of performance measurement data */
		void Clear();
		
		/* Retrieve the current estimate skill level */
		tdble GetSkillEstimate();
	};
}
#endif // _PM_MANAGER_H_