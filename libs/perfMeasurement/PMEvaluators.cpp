/*
	File: PMEvaluationFunctions.cpp
	Author: Henri Keeble
	Desc: Definitions for all evaluation object types. Each has it's own update and evaluation behaviours.
*/

#include "perfMeasurement\PMEvaluators.h"

namespace perfMeasurement
{
	/*-----------------------------------------\
	| MEAN DEVIATION FROM TOP SPEED BEHAVIOURS |
	\-----------------------------------------*/
	void MeanDeviationFromTopSpeed::Update(tdble deltaTimeIncrement, tdble currentTime)
	{
		cumulativeTime += deltaTimeIncrement;

		car.Update();

		if (cumulativeTime >= UPDATE_INTERVAL)
		{
			dataSet.AddData(car, currentTime); // Add new data to collection

			timeOnLastUpdate = currentTime;
			cumulativeTime = 0.0f;
		}
	}

	tdble MeanDeviationFromTopSpeed::Evaluate()
	{
		float tot = 0.f;
		for (int i = 0; i < dataSet.Count(); i++)
			tot += dataSet(i).Data().Speed();

		tot -= dataSet(0).Data().GetCar()->race.topSpeed; // Is this top speed in race or overall??

		tot = tot / dataSet.Count();
		return tot / 100;
	}

	/*-----------------------------------------\
	|		RACE LINE EVALUATION BEHAVIOURS	    |
	\-----------------------------------------*/
	void RaceLineEvaluation::Update(tdble deltaTimeIncrement, tdble currentTime)
	{
		car.Update();

		// Retrieve types of surrouding segments
		int currentType = car.CurrentSeg()->type;
		int nextType = car.CurrentSeg()->next->type;
		int prevType = car.CurrentSeg()->prev->type;

		// Switch is based on the current type, decision is made here in what positions to record data.
		switch (currentType)
		{
		case TR_STR:
			if (nextType == TR_LFT || TR_RGT)
			{
				// If car is near end, record position
			}
			break;
		case TR_LFT:
			// If car is near center, record position
			break;
		case TR_RGT:
			// If car is near center, record position
			break;
		}

		// Determine if new evaluation needs to take place. That is, a corner has just been passed by.
		if (currentType == TR_STR && prevType == TR_RGT || prevType == TR_LFT)
		{
			// Make new evaluation based on dataset
		}
	}

	tdble RaceLineEvaluation::Evaluate()
	{
		// Declarations
		tdble rdiff = 0.f;
		tTrkLocPos* optimalPositions;
		tTrkLocPos* actualPositions;
		tdble optimalRadius = 0.f;
		tdble actualRadius = 0.f;
		tTrackSeg* curSeg;

		if (dataSet.Count() > 0)
		{
			// Retrieve optimal positions
			for (int i = 0; i < dataSet.Count(); i++)
			{
				curSeg = dataSet(i).Data().CurrentSeg(); // Segment contained in this set of data
				switch (curSeg->type)
				{
				case TR_STR:
					break;
				case TR_LFT:
					break;
				case TR_RGT:
					break;
				}
			}

			// Use this to calculate optimal radius

			// Get actual recorded positions
			actualPositions = new tTrkLocPos[dataSet.Count()];
			for (int i = 0; i < dataSet.Count(); i++)
				actualPositions[i] = dataSet(i).Data().LocalPosition();

			// Then use data on positions stored in data set to calculate the player's arc radius

			// Calculate differences
			rdiff = optimalRadius - actualRadius; // Difference between radii
			if (rdiff < 0) // Ensure a positive value
				rdiff *= -1;

			dataSet.Clear(); // Clear previous cornering data at the end of each evaluation
		}
		else
			pmOut("Error in race line evaluation. Evaluation function called with empty data set. Returning 0 as skill evaluation.\n");

		// Delete values not used outside this scope
		if (optimalPositions)
			delete[] optimalPositions;
		if (actualPositions)
			delete[] actualPositions;

		return rdiff;
	}
}