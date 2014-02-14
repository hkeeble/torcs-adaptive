/*
	File: PMManager.h
	Author: Henri Keeble
	Desc: Declaration for performance measurement management class.
*/

#ifndef _PM_MANAGER_H_
#define _PM_MANAGER_H_

#include "PMData.h"
#include "PMEvaluationFunctions.h"
#include "car.h"

namespace perfMeasurement
{
	#define PERFMEASURE_UPDATE_INTERVAL 0.4

	/* Class used as interface to performance measurement in a game session. */
	class PMManager
	{
	private:
		CarData car;
		PMDataCollection data;
		int skillEstimate;
		
		double cumulativeTime;
		double timeOnLastUpdate;
		
		/* Functor pointer to current evaluation functor */
		PMEvaluationFunctor* Evaluate;

	public:
		PMManager(CarElt* car);
		~PMManager();
		PMManager(const PMManager& param);
		PMManager& operator=(const PMManager& param);
		
		/* Get the car currently being monitored */
		const CarElt* GetCar();
		
		/* Update the performance measurement data */
		void Update(double deltaTimeIncrement, double currentTime);
		
		/* Clear the current collection of performance measurement data */
		void Clear();
		
		/* Retrieve the current estimate skill level */
		float GetSkillEstimate();
	};
}
#endif // _PM_MANAGER_H_