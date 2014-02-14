/*
	File: PMManager.cpp
	Author: Henri Keeble
	Desc: Definitions for a performance measurement manager class.
*/

#include "perfMeasurement\PMManager.h"
#include "Util.h"

namespace perfMeasurement
{
	PMManager::PMManager(CarElt* car)
	{
		this->car = car;
		skillEstimate = 0;
		cumulativeTime = 0.0f;
		timeOnLastUpdate = 0.0f;
	}

	PMManager::~PMManager()
	{

	}

	PMManager::PMManager(const PMManager& param)
	{
		skillEstimate = param.skillEstimate;
		car = param.car;
	}

	PMManager& PMManager::operator=(const PMManager& param)
	{
		if(&param == this)
			return *this;
		else
		{
			skillEstimate = param.skillEstimate;
			car = param.car;
			return *this;
		}
	}

	const CarElt* PMManager::GetCar()
	{
		return car.GetCar();
	}

	void PMManager::Update(double deltaTimeIncrement, double currentTime)
	{
		cumulativeTime += deltaTimeIncrement;

		car.Update();

		if(cumulativeTime >= PERFMEASURE_UPDATE_INTERVAL)
		{
			data.AddData(car, currentTime); // Add new data to collection

			if (data.Count() == data.MaximumDataSets()) // Only begin evaluation with a full set of data
				skillEstimate = (*Evaluate)(data);

			timeOnLastUpdate = currentTime;
			cumulativeTime = 0.0f;
		}
	}

	void PMManager::Clear()
	{
		data.Clear();
	}

	float PMManager::GetSkillEstimate()
	{
		return skillEstimate;
	}
}