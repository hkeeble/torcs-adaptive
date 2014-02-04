/*
File: TorcsAdaptiveManager.h
Author: Henri Keeble
Desc: Defines a singleton class that manages TORCS-Adaptive specific races. See header file for function explainations.
*/
#include "TAManager.h"
#include "procedural\trackdesc.h"

namespace torcsAdaptive
{
	TAManager* TAManager::instance = nullptr;

	TAManager::TAManager()
	{
		raceType = TARaceType::None;
		perfMeasurement = nullptr;
		trackManager = nullptr;
		currentSkillLevel = 0.f;
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

	void TAManager::Init(tRmInfo* RaceManager)
	{
		raceManager = RaceManager;

		// Initialize Race Mode for TORCS Adaptive
		if (strcmp(raceManager->raceEngineInfo.name, "Adaptive Race") == 0)
			SetRaceType(torcsAdaptive::TARaceType::Adaptive);
		else if (strcmp(raceManager->raceEngineInfo.name, "Procedural Race") == 0)
			SetRaceType(torcsAdaptive::TARaceType::Procedural);
		else
			SetRaceType(torcsAdaptive::TARaceType::None);

		if (raceManager->raceEngineInfo.displayMode == RM_DISP_MODE_CONSOLE)
			raceOnConsole = true;
		else
			raceOnConsole = false;
	}

	void TAManager::InitGraphics()
	{
		// Initialize Graphics
		PTrack* trInfo = raceManager->_reTrackItf.PGetTrackInfo(); // Retrieve track info
		if (raceManager->_displayMode != RM_DISP_MODE_CONSOLE) // Load Initial 3D decsription if not displaying on console
		{
			trInfo->SetTrackDesc(raceManager->_reGraphicItf.pLoad3DDesc(trInfo->GetACName(), (ssgLoaderOptions*)trInfo->GetLoaderOptions()));
			raceManager->_reGraphicItf.pAttach3DDesc(trInfo->GetTrackDesc()); // Attach 3D description to scene graph
		}
	}

	void TAManager::InitTrack()
	{
		raceManager->track = raceManager->_reTrackItf.PTrackInit(TA_TR_LENGTH, raceOnConsole);
	}


	void TAManager::InitTrkManager(tCarElt* car, PTrack* Track)
	{
		if (trackManager)
			delete trackManager;

		trackManager = new PTrackManager();
		trackManager->Init(car, raceManager, Track);

		this->car = car;
		isAddingSegments = true;
	}

	void TAManager::InitPerfMeasurement(tCarElt* car)
	{
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
		if (raceType == TARaceType::Adaptive)
		{
			trackManager->Update(currentSkillLevel);
			currentSkillLevel = perfMeasurement->GetSkillEstimate();
		}
		else
		{
			trackManager->Update();
			if (!raceOnConsole)
				trackManager->UpdateGraphics();
		}
	}

	void TAManager::SetRaceType(const TARaceType& RaceType)
	{
		raceType = RaceType;
	}

	TARaceType TAManager::GetRaceType() const
	{
		return raceType;
	}

	bool TAManager::IsActive() const
	{
		if (raceType == TARaceType::None)
			return false;
		else
			return true;
	}

	void TAManager::CheckIfFinished()
	{
		if (trackManager->CurrentLength() >= trackManager->TotalLength())
		{
			isAddingSegments = false;
			if (trackManager->CarOnLastSegment())
				raceManager->s->raceInfo.state = RM_RACE_ENDED;
		}
	}

	void TAManager::RaceEnd()
	{
		/* Build the track in it's entirety, and then plot the track for reading in gnuplot. */
		tTrack* trk = trackManager->BuildTrack();
		TrackDesc desc = TrackDesc(trk);
		desc.plot("procTrackPlot.dat");
	}
}