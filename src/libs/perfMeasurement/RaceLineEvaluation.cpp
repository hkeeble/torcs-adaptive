#include "perfMeasurement\RaceLineEvaluation.h"
#include "K1999.h"
#include "BerniwPathfinding.h"

namespace perfMeasurement
{
	RaceLineEvaluation::RaceLineEvaluation(tCarElt* car, Pathfinder pathfinder, tTrack* track, tSituation* situation) : PMEvaluator(car)
	{
		// Initialize track description
		trackDesc = new PTrackDesc(track);

		// Initialize pathfinder algo
		switch (pathfinder)
		{
		case Pathfinder::K1999:
			this->pathfinder = new K1999(trackDesc);
			break;
		case Pathfinder::Berniw:
			this->pathfinder = new BerniwPathfinding(trackDesc);
			break;
		default:
			pmOut("Error, unrecognized pathfinder type passed into race line evaluation for performance measurement.");
			break;
		}

		// Initialize car description
		this->carDesc = new PCarDesc(this->pathfinder, this->GetCar(), situation);

		currentSegmentID = 0; // ID of the current segment ID being evaluated
		currentPathSegID = 0; // ID of the current path segment

		actualPoints = std::vector<PMPoint2D>();
	}

	void RaceLineEvaluation::Evaluate()
	{
		// Evaluate the path here, perhaps when the car reaches the end of a segment.
		/*
			Considerations:
			* Average distance from path across segment, need to collect positional data at set intervals
			* Average distance from optimal speed at each point, again need to collect velocity data at set intervals
			* Need to convert the result into a ratio to work correctly with segment generation, must weight both speed and distance
			  from line appropriately.
		*/

		tdble avgDistDiff, avgSpdDistDiff;
		tdble totalDistDiff = 0, totalSpdDiff = 0;

		for (auto d : dataSet)
		{
			totalSpdDiff += abs(pathfinder->Seg(currentSegmentID)->getSpeedsqr() - d.Data().Speed());
			totalDistDiff += abs(distToPath(trackDesc, pathfinder->Segs(), currentSegmentID, new v3d(d.Data().GetCar()->_pos_X,
										d.Data().GetCar()->_pos_Y, d.Data().GetCar()->_pos_Z)));
		}

		// Calculate average distance from path over segment and average different from optimal speed
		avgDistDiff = totalDistDiff / dataSet.size();
		avgSpdDistDiff = totalSpdDiff / dataSet.size();
	}

	void RaceLineEvaluation::Update(tdble deltaTimeIncrement, tdble currentTime)
	{
		trackDesc->Update();
		pathfinder->Update(carDesc); // Need to add a situation somehow...

		// Collect data here, and when the end of a segment is reached call evaluation function to evaluate collected data
		
		car.Update(); // Update car data

		// If the car is on a new path segment, add new data
		int prev = currentPathSegID;
		currentPathSegID = pathfinder->getCurrentSegment(car.GetCar());
		if (prev < currentPathSegID)
		{
			dataSet.push_back(PMData(car, currentTime));
		}

		if (car.LocalPosition().seg->id > currentSegmentID)
		{
			Evaluate();
			currentSegmentID++;

			for (auto d : dataSet)
			{
				actualPoints.push_back(PMPoint2D(d.Data().GlobalPosition().x, d.Data().GlobalPosition().y));
			}

			dataSet.clear();
		}
	}

	void RaceLineEvaluation::RaceEnd()
	{
		remove("optimal.dat");
		remove("actual.dat");

		pathfinder->PlotPath("optimal.dat");

		std::fstream out = std::fstream("actual.dat", std::ios::app);
		if (out.is_open())
		{
			for (auto point : actualPoints)
			{
				out << point.x << "," << point.y << std::endl;
			}
			out.close();
		}
	}
}