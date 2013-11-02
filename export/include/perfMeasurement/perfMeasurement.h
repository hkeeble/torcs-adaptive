/*
	File: perfMeasurement.h
	Desc: Declaration for performance measurement class that uses telemetry to measure player performance.
	Author: Henri Keeble
	Created: 02/10/2013
	Last Edit: 02/10/2013
*/

#ifndef _PERF_MEASUREMENT_H
#define _PERF_MEASUREMENT_H

#include "drvTel.h"
#include "car.h"

#ifdef PERFMEASUREMENT_EXPORT
#define PERFMEASUREMENT_API __declspec(dllexport)
#else
#define PERFMEASUREMENT_API __declspec(dllimport)
#endif // PERFMEASUREMENT_EXPORT

#define UPDATE_INTERVAL 0.4

/* Class used as interface to performance measurement in a game session. */
class taPerfMeasurement
{
public:
	taPerfMeasurement();
	~taPerfMeasurement();
	taPerfMeasurement(const taPerfMeasurement& param);
	taPerfMeasurement& operator=(const taPerfMeasurement& param);
	
	void SetDriver(CarElt* car);
	void* getDataFromTag(std::string tag);
	void Update();
	void Clear();
	int GetSkillEstimate();
	
private:
	void Evaluate();
	drvTelemetry driver;
	int skillEstimate;
};

#endif // _PERF_MEASUREMENT_H