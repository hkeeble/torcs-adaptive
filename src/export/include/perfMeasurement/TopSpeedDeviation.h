/**
 *	@file 	TopSpeedDeviation.h
 *	@author Henri Keeble
 *	@brief 	Declares an evaluation class for monitoring deviation from top speed.
*/

#ifndef _TOP_SPEED_DEVIATION_H_
#define _TOP_SPEED_DEVIATION_H_

#include "PMEvaluator.h"

/**
 * The namespace containing all performance measurement classes.
 */
namespace perfMeasurement
{
	/*! 
	 * Evaluates performance based upon the mean deviation from the car's top speed.
	 */
	class MeanDeviationFromTopSpeed : public PMEvaluator
	{
	private:
		tdble cumulativeTime; /*!< The total cumulative time. */
		tdble timeOnLastUpdate; /*!< The time on the last call to update. */
		const tdble UPDATE_INTERVAL; /*!< The time interval on data collection. */
		virtual void Evaluate() override final; /*!< Override for evaluation behaviour. */
	public:
		MeanDeviationFromTopSpeed() : UPDATE_INTERVAL(0.1f), timeOnLastUpdate(0), cumulativeTime(0) { }
		
		//! Update the performance evaluator.
		/*!
			\param deltaTimeIncrement The delta time increment since the last call to update.
			\param currentTime		  The current total time.
		*/
		virtual void Update(tdble deltaTimeIncrement, tdble currentTime) override final;
	};
}

#endif // _TOP_SPEED_DEVIATION_H_