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
	void RaceLineEvaluation::CornerOutlook::GenerateOutlook(tTrackSeg* baseSeg, tdble carWidth, tdble carDepth)
	{
		if (!clear)
			Clear();

		pmOut("Generating new corner outlook for race line evaluation.\n");

		// Initialize segment pointers
		entrance.seg = baseSeg;
		corner.seg = baseSeg->next;
		exit.seg = corner.seg->next;

		// Calculate data recording ranges, relative to car distance to start. When distance to start is between these ranges, data is recorded on this segment
		entrance.minRange = 0;
		entrance.maxRange = 10;

		exit.minRange = 0;
		exit.maxRange = 10;

		corner.minRange = (corner.seg->length / 2 - 10) / corner.seg->radius;
		corner.maxRange = (corner.seg->length / 2 + 10) / corner.seg->radius;

		// Calculate optimal arc radius
		optimalRadius = CalculateOptimalRadius(carWidth, carDepth);

		clear = false;
	}

	tdble RaceLineEvaluation::CornerOutlook::CalculateOptimalRadius(tdble carWidth, tdble carDepth)
	{
		tdble rad = 0.f;

		// Perform Calculations
		PMPoint2D* optimalPoints = CalculateOptimalPoints(carWidth, carDepth);
		if (optimalPoints != nullptr)
		{
			// Get line segments between optimal points
			PMLineSeg* lineSegs = new PMLineSeg[NUMB_OF_OPTIMAL_POINTS - 1];
			for (int i = 0; i < NUMB_OF_OPTIMAL_POINTS - 1; i++)
				lineSegs[i] = PMLineSeg(optimalPoints[i], optimalPoints[i + 1]);

			// Find the perpendicular bisector of the lines
			PMLineSeg* bisectingLines = new PMLineSeg[NUMB_OF_OPTIMAL_POINTS - 1];
			for (int i = 0; i < NUMB_OF_OPTIMAL_POINTS - 1; i++)
				bisectingLines[i] = PMMath::CalculatePerpendicularBisector(lineSegs[i]);

			// Turn line segments into lines
			PMLine* lines = new PMLine[NUMB_OF_OPTIMAL_POINTS - 1];
			for (int i = 0; i < NUMB_OF_OPTIMAL_POINTS - 1; i++)
				lines[i] = bisectingLines[i];

			// Find intersection point of the lines
			if (lines[0].Intersects(lines[1]))
			{
				PMPoint2D intersectionPoint = lines[0].IntersectionPoint(lines[1]);
				rad = PMMath::DistBetweenPoints(intersectionPoint, *optimalPoints);
			}
			else
			{
				pmOut("WARNING: Unable to find intersection point between perpendicular bisectors of the optimal path arc chords.\n");
				rad = 0.f;
			}

			// Delete unused data
			delete[] lines;
			delete[] bisectingLines;
			delete[] lineSegs;
			delete[] optimalPoints;

			return rad;
		}
		else
		{
			pmOut("Unable to calculate optimal radius, returning 0.f.\n");
			return 0.f;
		}
	}

	PMPoint2D* RaceLineEvaluation::CornerOutlook::CalculateOptimalPoints(tdble carWidth, tdble carDepth)
	{
		// Indices in arrays for optimal points
		const int EN = 0;
		const int CR = 1;
		const int EX = 2;

		// Total number of optimal points
		const int TOT = NUMB_OF_OPTIMAL_POINTS;

		// Optimal Point Arrays (global is calculated from local dependent on segment)
		PMPoint2D* globalOptimalPoints = new PMPoint2D[TOT];
		tTrkLocPos* localOptimalPoints = new tTrkLocPos[TOT];


		// Half of car width
		tdble hcw = carWidth / 2;

		// Half of car depth
		tdble hcd = carDepth / 2;

		// Initialize all points
		for (int i = 0; i < TOT; i++)
		{
			globalOptimalPoints[i] = PMPoint2D();
			localOptimalPoints[i] = tTrkLocPos();
		}

		// Assign segments
		localOptimalPoints[EN].seg = entrance.seg;
		localOptimalPoints[CR].seg = corner.seg;
		localOptimalPoints[EX].seg = exit.seg;

		// To Start and To Middle (not dependent on corner direction)
		
		// Entry Point
		localOptimalPoints[EN].toStart = 0;
		localOptimalPoints[EN].toMiddle = (entrance.seg->width / 2) - hcw;

		// Exit Point
		localOptimalPoints[EX].toStart = exit.seg->length;
		localOptimalPoints[EX].toMiddle = (entrance.seg->width / 2) - hcw;

		// Apex Point
		localOptimalPoints[CR].toStart = ((corner.seg->length / 2) - hcd) / corner.seg->radius;
		localOptimalPoints[CR].toMiddle = (entrance.seg->width / 2) - hcw;

		// Calculate toLeft and toRight distances dependent on corner direction
		switch (corner.seg->type)
		{
		case TR_RGT:
			// Entry Point
			localOptimalPoints[EN].toLeft = hcw;
			localOptimalPoints[EN].toRight = entrance.seg->width - hcw;

			// Exit Point
			localOptimalPoints[EX].toLeft = hcw;
			localOptimalPoints[EX].toRight = exit.seg->width - hcw;

			// Apex Point
			localOptimalPoints[CR].toLeft = corner.seg->width - hcw;
			localOptimalPoints[CR].toRight = hcw;
			break;
		case TR_LFT:
			// Entry Point
			localOptimalPoints[EN].toRight = hcw;
			localOptimalPoints[EN].toLeft = entrance.seg->width - hcw;

			// Exit Point
			localOptimalPoints[EX].toRight = hcw;
			localOptimalPoints[EX].toLeft = exit.seg->width - hcw;

			// Apex Point
			localOptimalPoints[CR].toLeft = hcw;
			localOptimalPoints[CR].toRight = corner.seg->width - hcw;
			break;
		default:
			pmOut("Cannot calculate optimal radius for corner outlook, corner of unrecognized type.");
			return nullptr;
			break;
		}

		// Calculate and assign global positions
		for (int i = 0; i < TOT; i++)
			RtTrackLocal2Global(&localOptimalPoints[i], &globalOptimalPoints[i].x, &globalOptimalPoints[i].y, TR_TORIGHT);

		delete[] localOptimalPoints;

		return globalOptimalPoints;
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
		{
			if (car.CurrentSeg()->next->type != TR_STR)
			{
				currentOutlook.GenerateOutlook(car.CurrentSeg(), car.GetCar()->info.dimension.x, car.GetCar()->info.dimension.z);
				pmOut("Calculated optimal radius for upcoming corner as: " + std::to_string(currentOutlook.optimalRadius) + "\n");
			}
		}
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
		currentOutlook.Clear();
		return NULL_SKILL_LEVEL;
	}
}