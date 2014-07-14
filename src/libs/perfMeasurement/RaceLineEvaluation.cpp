#include "perfMeasurement\RaceLineEvaluation.h"
#include "K1999.h"
#include "BerniwPathfinding.h"

namespace perfMeasurement
{
	RaceLineEvaluation::RaceLineEvaluation(tCarElt* car, Pathfinder pathfinder, tTrack* track, tSituation* situation) : PMEvaluator(car), speedWeight(0.4), positionWeight(0.6)
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

		actualData = std::vector<PMData>();
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

		tdble avgDistDiff = 0, avgDistMax = 0;
		tdble totalDistDiff = 0, totalDistMax = 0;

		tdble totalSpeed = 0, desiredTotalSpeed = 0;
		tdble avgSpeed = 0, desiredAvgSpeed = 0;

		for (auto d : dataSet)
		{
			int currentPathSeg = pathfinder->getCurrentSegment(&d.GetCar()); // Path segment in this data set
			int currentTrackSeg = trackDesc->getCurrentSegment(&d.GetCar()); // Track segment in this data set

			tdble dist = fabs(distToPath(trackDesc, pathfinder->Segs(), currentTrackSeg, new v3d(d.GetCar()._pos_X, d.GetCar()._pos_Y, d.GetCar()._pos_Z)));
			totalDistDiff += dist;

			// Total maximum possible distances
			PTrackSegment* nearestSeg = trackDesc->getSegmentPtr(currentTrackSeg);
			tdble optLocX = pathfinder->Seg(currentPathSeg)->getOptLoc()->x;
			tdble optLocY = pathfinder->Seg(currentPathSeg)->getOptLoc()->y;
			tTrkLocPos pos;
			RtTrackGlobal2Local(nearestSeg->getTrackSegment(), optLocX, optLocY, &pos, TR_LPOS_MAIN);

			if (pos.toLeft > pos.toRight)
				totalDistMax += pos.toLeft;
			else
				totalDistMax += pos.toRight;

			// Speed weighting
			totalSpeed += d.GetData().Speed();
			desiredTotalSpeed += pathfinder->Seg(currentPathSeg)->getSpeedsqr();
		}

		// Calculate average distance from path over segment and average different from optimal speed
		avgDistDiff = totalDistDiff / dataSet.size();
		avgDistMax = totalDistMax / dataSet.size();
		
		tdble positionRating = 1 - (avgDistDiff / avgDistMax); // The position rating for this estimation
		tdble speedRating = (totalSpeed / dataSet.size()) / (desiredTotalSpeed / dataSet.size());

		currentEstimate = (positionWeight * positionRating) + (speedWeight * speedRating); // Produce final estimate using weighted mean
	}

	void RaceLineEvaluation::Update(tdble deltaTimeIncrement, tdble currentTime)
	{
		trackDesc->Update();
		pathfinder->Update(carDesc);
		
		// If the car is on a new path segment, add new data
		int prev = currentPathSegID;
		currentPathSegID = pathfinder->getCurrentSegment(car);

		if (prev < currentPathSegID)
			dataSet.push_back(PMData(car, currentTime));

		if (car->pub.trkPos.seg->id > currentSegmentID)
		{
			Evaluate();
			currentSegmentID++;

			// Record all data for future reference before clearing the current data set
			for (auto d : dataSet)
				actualData.push_back(d);

			dataSet.clear();
		}
	}

	void RaceLineEvaluation::RaceEnd()
	{
		remove("optimal.dat");
		remove("actual.dat");
		remove("optSpeed.dat");
		remove("actSpeed.dat");

		pathfinder->PlotPath("optimal.dat");

		std::fstream out = std::fstream("actual.dat", std::ios::app);
		if (out.is_open())
		{
			for (auto data : actualData)
				out << data.GetData().GlobalPosition().x << "," << data.GetData().GlobalPosition().y << std::endl;

			out.close();
		}

		out = std::fstream("optSpeed.dat", std::ios::app);
		if (out.is_open())
		{
			for (int i = 0; i < pathfinder->Segs().Count(); i++)
				out << sqrt(pathfinder->Seg(i)->getSpeedsqr()*3.6) << std::endl;

			out.close();
		}

		out = std::fstream("actSpeed.dat", std::ios::app);
		if (out.is_open())
		{
			for(auto data : actualData)
				out << data.GetData().Speed() << std::endl;

			out.close();
		}
	}
}