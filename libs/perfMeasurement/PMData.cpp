/*
	File: PMData.cpp
	Author: Henri Keeble
	Desc: Definitions for a performance data class and a collection of these objects.
*/

#include "perfMeasurement\PMData.h"

namespace perfMeasurement
{
	PMData::PMData(CarData newData, float timeStamp)
	{

	}

	PMData::~PMData()
	{

	}

	CarData PMData::Data() const
	{

	}

	float PMData::TimeStamp() const
	{

	}

	PMDataCollection::PMDataCollection()
	{

	}

	PMDataCollection::PMDataCollection(const PMDataCollection& param)
	{

	}

	PMDataCollection& PMDataCollection::operator=(const PMDataCollection& param)
	{

	}

	PMDataCollection::~PMDataCollection()
	{

	}

	int PMDataCollection::Count() const
	{

	}

	void PMDataCollection::AddData(const CarData& data, float timeStamp)
	{

	}

	const PMData& PMDataCollection::operator()(int index)
	{

	}

	const PMData& PMDataCollection::LatestData()
	{

	}

	void PMDataCollection::Clear()
	{

	}
}