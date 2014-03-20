/*
	File: PMData.cpp
	Author: Henri Keeble
	Desc: Definitions for a class that holds a single set of performance measurement data.
*/

#include "perfMeasurement\PMData.h"

namespace perfMeasurement
{
	PMData::PMData(CarData newData, tdble timeStamp)
	{
		data = newData;
		currentTime = timeStamp;
	}

	PMData::~PMData()
	{

	}

	CarData PMData::Data() const
	{
		return data;
	}

	tdble PMData::TimeStamp() const
	{
		return currentTime;
	}
}