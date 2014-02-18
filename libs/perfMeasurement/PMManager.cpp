/*
	File: PMManager.cpp
	Author: Henri Keeble
	Desc: Definitions for a performance measurement manager class.
*/

#include "perfMeasurement\PMManager.h"
#include "Util.h"

namespace perfMeasurement
{
	PMManager* PMManager::instance = nullptr;

	PMManager* PMManager::Get()
	{
		if (instance)
			return instance;
		else
		{
			instance = new PMManager();
			return instance;
		}
	}

	PMManager::~PMManager()
	{
		if (Evaluator)
			delete Evaluator;
	}

	tCarElt* PMManager::GetCar()
	{
		return Evaluator->GetCar();
	}

	void PMManager::Update(tdble deltaTimeIncrement, tdble currentTime)
	{
		Evaluator->Update(deltaTimeIncrement, currentTime);
		skillEstimate = Evaluator->Evaluate();
		pmOut("Re-evaluating skill level...\n\t\tNew Skill Level Estimate: " + std::to_string(skillEstimate) + "\n");
	}

	void PMManager::Clear()
	{
		Evaluator->ClearData();
	}

	tdble PMManager::GetSkillEstimate()
	{
		return skillEstimate;
	}

	void PMManager::Init(tCarElt* car, PMEvaluator* evaluator)
	{
		Evaluator = evaluator;
		Evaluator->Init(car);
	}
}