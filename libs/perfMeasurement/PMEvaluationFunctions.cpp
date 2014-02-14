/*
	File: PMEvaluationFunctions.cpp
	Author: Henri Keeble
	Desc: Definitions for all evaluation function types.
*/

#include "perfMeasurement\PMEvaluationFunctions.h"

namespace perfMeasurement
{
	
	float MeanDeviationFromTopSpeed::operator()(PMDataCollection& dataSet)
	{
		float tot = 0.f;
		for (int i = 0; i < dataSet.Count(); i++)
			tot += dataSet(i).Data().GetCar()-
	}

	float RaceLineEvaluation::operator()(PMDataCollection& dataSet)
	{
		
	}
}