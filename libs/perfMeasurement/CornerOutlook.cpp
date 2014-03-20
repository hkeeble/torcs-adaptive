/*
	File: CornerOutlook.cpp
	Author: Henri Keeble
	Desc: Defines a class to maintain and evaluate an outlook on a corner.
*/
#include "perfMeasurement\RaceLineEvaluation.h"

namespace perfMeasurement
{
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
		PMPoint2D* optimalPoints = CalculateOptimalPoints(carWidth, carDepth);
		optimalRadius = abs(radius(optimalPoints[0].x, optimalPoints[0].y, optimalPoints[1].x, optimalPoints[1].y, optimalPoints[2].x, optimalPoints[2].y));

		clear = false;
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

		// Exit Point
		localOptimalPoints[EX].toStart = exit.seg->length;

		// Apex Point
		localOptimalPoints[CR].toStart = ((corner.seg->length / 2) - hcd) / corner.seg->radius;

		// Calculate toLeft and toRight distances dependent on corner direction
		switch (corner.seg->type)
		{
		case TR_RGT:
			// Entry Point
			localOptimalPoints[EN].toRight = entrance.seg->width - hcw;

			// Exit Point
			localOptimalPoints[EX].toRight = exit.seg->width - hcw;

			// Apex Point
			localOptimalPoints[CR].toRight = hcw;
			break;
		case TR_LFT:
			// Entry Point
			localOptimalPoints[EN].toRight = hcw;

			// Exit Point
			localOptimalPoints[EX].toRight = hcw;

			// Apex Point
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

	void RaceLineEvaluation::CornerOutlook::Segment::Update(CarData& data, std::vector<PMData>& dataSet, const tdble& currentTime)
	{
		if (data.LocalPosition().toStart > minRange && data.LocalPosition().toStart < maxRange)
		{
			dataSet.push_back(PMData(data, currentTime));
			pmOut("Adding data to data set for race line evaluation.\n");
		}
		else if (data.LocalPosition().toStart > maxRange)
		{
			dataSet.push_back(PMData(data, currentTime));
			pmOut("Car already passed data collection range, approximating position.");
		}
	}

	void RaceLineEvaluation::CornerOutlook::Update(CarData& data, std::vector<PMData>& dataSet, const tdble& currentTime)
	{
		// Get the current speed data
		speedData.push_back(data.Speed());

		if (data.GetCar()->priv.dammage > 1)
			pmOut("TEST");

		// Update position data where neccesary
		if (data.CurrentSeg()->id == entrance.seg->id && dataSet.size() == 0)
			entrance.Update(data, dataSet, currentTime);
		else if (data.CurrentSeg()->id == corner.seg->id && dataSet.size() == 1)
			corner.Update(data, dataSet, currentTime);
		else if (data.CurrentSeg()->id == exit.seg->id && dataSet.size() == 2)
			exit.Update(data, dataSet, currentTime);
	}

	tdble RaceLineEvaluation::CornerOutlook::AverageSpeed()
	{
		tdble tot = 0;
		for (int i = 0; i < speedData.size(); i++)
			tot += speedData.at(i);
		return tot / speedData.size();
	}
}