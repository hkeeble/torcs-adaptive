/*
	File: perfMeasurementImp.h
	Desc: Definitions for a performance measurement class that uses telemetry to measure player performance.
	Author: Henri Keeble
	Created: 02/10/2013
	Last Edit: 02/10/2013
*/

#include "perfMeasurement\perfMeasurement.h"

PerfMeasurement::PerfMeasurement()
{

}

PerfMeasurement::~PerfMeasurement()
{

}

PerfMeasurement::PerfMeasurement(const PerfMeasurement& param)
{

}

PerfMeasurement& PerfMeasurement::operator=(const PerfMeasurement& param)
{
	return *this;
}

	
void PerfMeasurement::SetDriver(CarElt* car)
{

}

void* PerfMeasurement::getDataFromTag(std::string tag)
{
	return 0;
}

void PerfMeasurement::Update()
{

}

void PerfMeasurement::Clear()
{

}

int PerfMeasurement::GetSkillEstimate()
{
	return 0;
}


void PerfMeasurement::Evaluate()
{

}