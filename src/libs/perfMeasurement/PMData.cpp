/*
	File: PMData.cpp
	Author: Henri Keeble
	Desc: Definitions for a class that holds a single set of performance measurement data.
*/

#include "perfMeasurement\PMData.h"

namespace perfMeasurement
{
	PMData::PMData(tCarElt* car, tdble timeStamp)
	{
		this->data = CarData(car);
		currentTime = timeStamp;
	}

	PMData::~PMData()
	{

	}

	tCarElt PMData::GetCar()
	{
		return data.GetCar();
	}

	CarData PMData::GetData()
	{
		return data;
	}

	tdble PMData::TimeStamp() const
	{
		return currentTime;
	}
}