#include "perfMeasurement\RaceLineEvaluation.h"

namespace perfMeasurement
{
	RaceLineEvaluation::RaceLineEvaluation(Pathfinder* pathfinder)
	{
		this->pathfinder = pathfinder;
		pathfinder->Plan();
	}

	void RaceLineEvaluation::Evaluate()
	{

	}

	void RaceLineEvaluation::Update(tdble deltaTimeIncrement, tdble currentTime)
	{
		pathfinder->UpdatePlan();
	}
}