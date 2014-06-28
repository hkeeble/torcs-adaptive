#include "perfMeasurement\RaceLineEvaluation.h"
#include "K1999.h"
#include "BerniwPathfinding.h"

namespace perfMeasurement
{
	RaceLineEvaluation::RaceLineEvaluation(Pathfinder pathfinder, tTrack* track)
	{
		// Initialize track description
		trackDesc = new PTrackDesc(track);

		// Initialize pathfinder algo
		PPathfinder* ppathfinder;
		switch (pathfinder)
		{
		case Pathfinder::K1999:
			ppathfinder = new K1999(trackDesc);
			break;
		case Pathfinder::Berniw:
			ppathfinder = new BerniwPathfinding(trackDesc);
			break;
		default:
			pmOut("Error, unrecognized pathfinder type passed into race line evaluation for performance measurement.");
			break;
		}

		// Initialize car description
		this->carDesc = new PCarDesc(ppathfinder, this->GetCar(), nullptr);

		// Plan the initial path
		ppathfinder->Plan(carDesc);
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
	}

	void RaceLineEvaluation::Update(tdble deltaTimeIncrement, tdble currentTime)
	{
		trackDesc->Update();
		pathfinder->Update(nullptr, carDesc); // Need to add a situation somehow...

		// Collect data here, and when the end of a segment is reached call evaluation function to evaluate collected data
	}
}