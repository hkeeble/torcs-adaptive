/*
	File: TopSpeedDeviation.cpp
	Author: Henri Keeble
	Desc: Defines an evaluator class that monitors the target's deviation from it's top speed.
*/

#include "perfMeasurement\TopSpeedDeviation.h"

namespace perfMeasurement
{
	void MeanDeviationFromTopSpeed::Update(tdble deltaTimeIncrement, tdble currentTime)
	{
		cumulativeTime += deltaTimeIncrement;

		if (cumulativeTime >= UPDATE_INTERVAL)
		{
			dataSet.push_back(PMData(car, currentTime)); // Add new data to collection

			timeOnLastUpdate = currentTime;
			cumulativeTime = 0.0f;
		}

		if (dataSet.size() == 3)
			Evaluate();
	}

	void MeanDeviationFromTopSpeed::Evaluate()
	{
		pmOut("Evaluating skill level using mean deviation from top speed...\n");

		float tot = 0.f;
		for (int i = 0; i < dataSet.size(); i++)
			tot += dataSet[i].GetData().Speed();

		tot -= dataSet[0].GetData().GetCar().race.topSpeed; // Is this top speed in race or overall??

		tot = tot / dataSet.size();
		currentEstimate = tot / 100;
	}
}