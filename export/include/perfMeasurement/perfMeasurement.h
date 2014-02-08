/*
	File: perfMeasurement.h
	Desc: Declaration for performance measurement class that uses telemetry to measure player performance.
	Author: Henri Keeble
	Created: 02/10/2013
	Last Edit: 02/10/2013
*/

#ifndef _PERF_MEASUREMENT_H
#define _PERF_MEASUREMENT_H

#include "taWatch.h"
#include "car.h"

namespace torcsAdaptive
{
	#define PERFMEASURE_UPDATE_INTERVAL 0.4
	#define DATA_OUTPUT(tag, data, type) GfOut(("\t" tag ":\t" + dbleToStr(*STATIC_CAST(data, type)) + "\n").c_str());
	
	/* Class used as interface to performance measurement in a game session. */
	class taPerfMeasurement
	{
	public:
		taPerfMeasurement();
		~taPerfMeasurement();
		taPerfMeasurement(const taPerfMeasurement& param);
		taPerfMeasurement& operator=(const taPerfMeasurement& param);
	
		void SetDriver(CarElt* car);
		const CarElt* GetCar() const;
		void* getDataFromTag(std::string tag);
		void Update(double deltaTimeIncrement, double currentTime);
		void Clear();
		float GetSkillEstimate();

	private:
		void Evaluate();
		void OutputData(double currentTime);

		drvWatch driver;
		int skillEstimate;
		
		double cumulativeTime;
		double timeOnLastUpdate;
	};
}
#endif // _PERF_MEASUREMENT_H