#ifndef _RACE_LINE_EVALUATION_H_
#define _RACE_LINE_EVALUATION_H_

#include "PMEvaluator.h"
#include "Pathfinder.h"
#include "K1999RaceLine.h"

namespace perfMeasurement
{
	class RaceLineEvaluation : public PMEvaluator
	{
	protected:
		virtual void Evaluate() override final;
	private:
		Pathfinder* pathfinder;
	public:
		RaceLineEvaluation(Pathfinder* pathfinder);
	
		virtual void Update(tdble deltaTimeIncrement, tdble currentTime) override final;
	};
}

#endif // _RACE_LINE_EVALUATION_H_