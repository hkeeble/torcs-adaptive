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
	tdble MeanDeviationFromTopSpeed::Update(tdble deltaTimeIncrement, tdble currentTime)
	{
		cumulativeTime += deltaTimeIncrement;

		car.Update();

		if (cumulativeTime >= UPDATE_INTERVAL)
		{
			dataSet.AddData(car, currentTime); // Add new data to collection

			timeOnLastUpdate = currentTime;
			cumulativeTime = 0.0f;
		}

		if (dataSet.Count() == dataSet.MaximumDataSets())
			return Evaluate();
		else
			return NULL_SKILL_LEVEL;
	}

	tdble MeanDeviationFromTopSpeed::Evaluate()
	{
		pmOut("Evaluating skill level using mean deviation from top speed...\n");

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
	void RaceLineEvaluation::CornerOutlook::GenerateOutlook(tTrackSeg* baseSeg)
	{
		if (!clear)
			Clear();

		pmOut("Generating new corner outlook for race line evaluation.\n");

		// Initialize segment pointers
		entrance.seg = baseSeg;
		corner.seg = baseSeg->next;
		exit.seg = corner.seg->next;
		
		// Calculate data recording ranges, relative to car distance to start. When distance to start is between these ranges, data is recorded on this segment
		entrance.minRange = entrance.seg->length - 10;
		entrance.maxRange = entrance.seg->length;

		corner.minRange = (corner.seg->length / 2 - 10) / corner.seg->radius;
		corner.maxRange = (corner.seg->length / 2 + 10) / corner.seg->radius;

		exit.minRange = 0;
		exit.maxRange = 10;

		clear = false;
	}

	void RaceLineEvaluation::CornerOutlook::Segment::Update(CarData currentData, PMDataCollection& dataSet, tdble currentTime)
	{
		if (!dataRecorded)
		{
			if (currentData.LocalPosition().toStart > minRange && currentData.LocalPosition().toStart < maxRange)
			{
				dataSet.AddData(currentData, currentTime);
				dataRecorded = true;
				pmOut("Adding data to data set for race line evaluation.\n");
			}
		}
	}

	void RaceLineEvaluation::CornerOutlook::Update(CarData data, PMDataCollection& dataSet, tdble currentTime)
	{
		if (data.CurrentSeg()->id == entrance.seg->id)
			entrance.Update(data, dataSet, currentTime);
		else if (data.CurrentSeg()->id == corner.seg->id)
			corner.Update(data, dataSet, currentTime);
		else if (data.CurrentSeg()->id == exit.seg->id)
			exit.Update(data, dataSet, currentTime);
	}

	tdble RaceLineEvaluation::Update(tdble deltaTimeIncrement, tdble currentTime)
	{
		car.Update();

		// When neccesary, generate a new outlook
		if (currentOutlook.IsClear() == true)
			if (car.CurrentSeg()->next->type != TR_STR)
				currentOutlook.GenerateOutlook(car.CurrentSeg());

		if (currentOutlook.IsClear() == false)
			currentOutlook.Update(car, dataSet, currentTime);

		if (currentOutlook.IsComplete())
			return Evaluate();
		else
			return NULL_SKILL_LEVEL; // No current skill estimate
	}

	tdble RaceLineEvaluation::Evaluate()
	{
		pmOut("Evaluating skill level using race line evaluation...\n");

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

		currentOutlook.Clear();

		return rdiff;
	}
}