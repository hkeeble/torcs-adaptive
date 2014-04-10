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
	}

	void PMManager::Clear()
	{
		Evaluator->ClearData();
	}

	tdble PMManager::GetSkillEstimate()
	{
		return Evaluator->GetCurrentEstimate();
	}

	tdble PMManager::GetAverageEstimate()
	{
		return Evaluator->GetAverageEstimate();
	}

	void PMManager::Init(tCarElt* car, PMEvaluator* evaluator)
	{
		Evaluator = evaluator;
		Evaluator->Init(car);
	}

	void PMManager::OutputData(std::string filepath)
	{
		std::ofstream file;
		file.open(filepath);

		file << " ----- PERFORMANCE EVALUATION REPORT ----- " << "\n\n";

		// Output driver name
		file << "-- INFORMATION --\n";
		file << "Driver: " << Evaluator->GetCar()->info.name << "\n";
		file << "Car: " << Evaluator->GetCar()->info.carName << "\n";
		file << "Average Skill Estimate: " << Evaluator->GetAverageEstimate() << "\n";
		file << "Total number of Estimates: " << Evaluator->GetEstimates().size() << "\n\n";

		// Output all estimates in sequential order
		file << "-- RAW ESTIMATE DATA --\n";
		const std::vector<tdble> estimates = Evaluator->GetEstimates();
		for (auto &i : estimates)
			file << i << "\n";

		file.close();
	}
}