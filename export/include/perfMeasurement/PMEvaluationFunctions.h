/*
	File: PMEvaluationFunctions.h
	Author: Henri Keeble
	Desc: Declarations for all evaluation functor types.
*/

#ifndef _PM_EVALUATION_FUNCTIONS_H_
#define _PM_EVALUATION_FUNCTIONS_H_

#include "PMData.h"

namespace perfMeasurement
{
	/* Base functor class, forces all evaluation functions to return float */
	class PMEvaluationFunctor
	{
	public:
			PMEvaluationFunctor() { }
			virtual float operator()(PMDataCollection& dataSet) = 0;
	};
	
	/* Evaluates performance based upon the mean deviation from the car's top speed. */
	class MeanDeviationFromTopSpeed : public PMEvaluationFunctor
	{
	public:
			MeanDeviationFromTopSpeed() { }
			virtual float operator()(PMDataCollection& dataSet) override;
	};
	
	/* Evaluates performance based upon the car's deviation from the optimal racing line within the given data set. */
	class RaceLineEvaluation : public PMEvaluationFunctor
	{
	public:
			RaceLineEvaluation() { }
			virtual float operator()(PMDataCollection& dataSet) override;
	};
}

#endif //_PM_EVALUATION_FUNCTIONS_H_