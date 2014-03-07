/*
	File: PMData.cpp
	Author: Henri Keeble
	Desc: Definitions for a performance data class and a collection of these objects.
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

	PMDataCollection::PMDataCollection() : maxDataSets(5)
	{
		data = std::vector<PMData>();
		nOfDataSets = 0;
	}

	PMDataCollection::PMDataCollection(const PMDataCollection& param) : maxDataSets(5)
	{
		data = param.data;
		nOfDataSets = param.nOfDataSets;
	}

	PMDataCollection& PMDataCollection::operator=(const PMDataCollection& param)
	{
		if (this == &param)
			return *this;
		else
		{
			data = param.data;
			nOfDataSets = param.nOfDataSets;
			return *this;
		}
	}

	PMDataCollection::~PMDataCollection()
	{
		data.clear();
	}

	int PMDataCollection::Count() const
	{
		return nOfDataSets;
	}

	void PMDataCollection::AddData(const CarData& data, float timeStamp)
	{
		this->data.push_back(PMData(data, timeStamp));
		nOfDataSets++;

		// Ensure vector remains below the maximum number of data sets
		if (nOfDataSets > maxDataSets)
		{
			this->data.erase(this->data.begin()); // Remove data at start of vector
			nOfDataSets--;
		}
	}

	const PMData& PMDataCollection::operator()(int index)
	{
		return data.at(index);
	}

	const PMData& PMDataCollection::LatestData()
	{
		return data.at(maxDataSets - 1);
	}

	void PMDataCollection::Clear()
	{
		data.clear();
	}

	int PMDataCollection::MaximumDataSets() const
	{
		return maxDataSets;
	}
}