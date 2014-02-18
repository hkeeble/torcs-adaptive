/*
	File: PMEvaluationFunctions.h
	Author: Henri Keeble
	Desc: Declarations for all evaluation functor types.
*/

#ifndef _PM_EVALUATION_FUNCTIONS_H_
#define _PM_EVALUATION_FUNCTIONS_H_

#include "PMData.h"
#include "PMDefs.h"

namespace perfMeasurement
{
	/* 
		Base Evaluation class. The idea is to derive from this class with bespoke update and
		evaluation functions, such that a data set is maintained and evaluated correctly for each method
	*/
	class PMEvaluator
	{
	protected:
		CarData car;
		PMDataCollection dataSet;
	public:
		PMEvaluator() { }
		virtual void Update(tdble deltaTimeIncrement, tdble currentTime) = 0;
		virtual tdble Evaluate() = 0;
		tCarElt* GetCar() { return car.GetCar(); }
		void ClearData() { dataSet.Clear(); }
		void Init(tCarElt* carElt) { car = carElt; dataSet = PMDataCollection(); }
	};
	
	/* Evaluates performance based upon the mean deviation from the car's top speed. */
	class MeanDeviationFromTopSpeed : public PMEvaluator
	{
	private:
		tdble cumulativeTime;
		tdble timeOnLastUpdate;
		const tdble UPDATE_INTERVAL;
	public:
		MeanDeviationFromTopSpeed() : UPDATE_INTERVAL(0.1f), timeOnLastUpdate(0), cumulativeTime(0) { }
		virtual void Update(tdble deltaTimeIncrement, tdble currentTime) override final;
		virtual tdble Evaluate() override final;
	};
	
	/* Evaluates performance based upon the car's deviation from the optimal racing line within the given data set. */
	class RaceLineEvaluation : public PMEvaluator
	{
	public:
		RaceLineEvaluation() { }
		virtual void Update(tdble deltaTimeIncrement, tdble currentTime) override final;
		virtual tdble Evaluate() override final;
	};
}

#endif //_PM_EVALUATION_FUNCTIONS_H_