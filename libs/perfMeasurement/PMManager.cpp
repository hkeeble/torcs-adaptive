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
		if (param.car)
			car = param.car;
	}

	PMManager& PMManager::operator=(const PMManager& param)
	{
		if(&param == this)
			return *this;
		else
		{
			skillEstimate = param.skillEstimate;
			if (param.car)
				car = param.car;
			return *this;
		}
	}

	const CarElt* PMManager::GetCar() const
	{
		return car;
	}

	void PMManager::Update(double deltaTimeIncrement, double currentTime)
	{
		cumulativeTime += deltaTimeIncrement;

		if(cumulativeTime >= PERFMEASURE_UPDATE_INTERVAL)
		{
			Evaluate();

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

	void PMManager::Evaluate()
	{
		int eval = 0;
		// ENTER EVALUATION FUNCTION HERE
		skillEstimate = eval;
	}
}