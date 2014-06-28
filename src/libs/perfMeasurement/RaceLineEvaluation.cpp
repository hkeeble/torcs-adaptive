#include "perfMeasurement\RaceLineEvaluation.h"

namespace perfMeasurement
{
	RaceLineEvaluation::RaceLineEvaluation(PPathfinder* pathfinder)
	{
		this->pathfinder = pathfinder;
		this->carDesc = new PCarDesc(pathfinder, this->GetCar(), nullptr);
		pathfinder->Plan(carDesc);
	}

	void RaceLineEvaluation::Evaluate()
	{

	}

	void RaceLineEvaluation::Update(tdble deltaTimeIncrement, tdble currentTime)
	{

	}
}