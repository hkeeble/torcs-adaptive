/*
	File: TopSpeedDeviation.h
	Author: Henri Keeble
	Desc: Declares an evaluation class for monitoring deviation from top speed.
*/

#ifndef _TOP_SPEED_DEVIATION_H_
#define _TOP_SPEED_DEVIATION_H_

#include "PMEvaluator.h"

namespace perfMeasurement
{
	/* Evaluates performance based upon the mean deviation from the car's top speed. */
	class MeanDeviationFromTopSpeed : public PMEvaluator
	{
	private:
		tdble cumulativeTime;
		tdble timeOnLastUpdate;
		const tdble UPDATE_INTERVAL;
		virtual void Evaluate() override final;
	public:
		MeanDeviationFromTopSpeed() : UPDATE_INTERVAL(0.1f), timeOnLastUpdate(0), cumulativeTime(0) { }
		virtual void Update(tdble deltaTimeIncrement, tdble currentTime) override final;
	};
}

#endif // _TOP_SPEED_DEVIATION_H_