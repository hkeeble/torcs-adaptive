/*
	File: perfMeasurementImp.h
	Desc: Definitions for a performance measurement class that uses telemetry to measure player performance.
	Author: Henri Keeble
	Created: 02/10/2013
	Last Edit: 02/10/2013
*/

#include "perfMeasurement\perfMeasurement.h"

taPerfMeasurement::taPerfMeasurement()
{
	skillEstimate = 0;
	driver = drvTelemetry();
}

taPerfMeasurement::~taPerfMeasurement()
{

}

taPerfMeasurement::taPerfMeasurement(const taPerfMeasurement& param)
{
	skillEstimate = param.skillEstimate;
	driver = param.driver;
}

taPerfMeasurement& taPerfMeasurement::operator=(const taPerfMeasurement& param)
{
	if(&param == this)
		return *this;
	else
	{
		skillEstimate = param.skillEstimate;
		driver = param.driver;
		return *this;
	}
}

	
void taPerfMeasurement::SetDriver(CarElt* car)
{
	driver.SetDriver(car);
}

void* taPerfMeasurement::getDataFromTag(std::string tag)
{
	return driver.getDataFromTag(tag);
}

void taPerfMeasurement::Update()
{

}

void taPerfMeasurement::Clear()
{
	driver.Clear();
}

int taPerfMeasurement::GetSkillEstimate()
{
	return skillEstimate;
}


void taPerfMeasurement::Evaluate()
{
	int eval = 0;
	// ENTER EVALUATION FUNCTION HERE
	skillEstimate = eval;
}