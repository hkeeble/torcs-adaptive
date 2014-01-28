/*
File: TorcsAdaptiveManager.h
Author: Henri Keeble
Desc: Defines a singleton class that manages TORCS-Adaptive specific races. See header file for function explainations.
*/
#include "TAManager.h"

namespace torcsAdaptive
{
	TAManager* TAManager::instance = nullptr;

	TAManager::TAManager()
	{
		raceType = TARaceType::None;
		perfMeasurement = nullptr;
		trackManager = nullptr;
	}

	TAManager* TAManager::Get()
	{
		if (instance)
			return instance;
		else
		{
			instance = new TAManager();
			return instance;
		}
	}

	TAManager::~TAManager()
	{
		if (instance)
			delete instance;
	}

	const TARaceType& TAManager::Type() const
	{
		return raceType;
	}

	taPerfMeasurement* TAManager::PerformanceMeasurement() const
	{
		return perfMeasurement;
	}

	void TAManager::Init(tCarElt* car, tRmInfo* RaceManager, PTrack* Track)
	{
		if (trackManager)
			delete trackManager;

		trackManager = new PTrackManager();
		trackManager->Init(car, RaceManager, Track);

		if (perfMeasurement)
			delete perfMeasurement;

		perfMeasurement = new taPerfMeasurement();
		perfMeasurement->SetDriver(car);
	}

	void TAManager::AddSegment(const PSeg& segment)
	{
		trackManager->AddSegment(segment);
	}

	void TAManager::UpdateTrack()
	{
		trackManager->UpdateTrack();
	}

	void TAManager::SetRaceType(const TARaceType& RaceType)
	{
		raceType = RaceType;
	}

	bool TAManager::IsActive() const
	{
		if (raceType == TARaceType::None)
			return false;
		else
			return true;
	}
}