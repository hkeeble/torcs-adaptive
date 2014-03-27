/*
File: TorcsAdaptiveManager.h
Author: Henri Keeble
Desc: Defines a singleton class that manages TORCS-Adaptive specific races. See header file for function explainations.
*/
#include "TAManager.h"
#include "procedural\trackdesc.h"
#include "../../client/mainmenu.h"

namespace torcsAdaptive
{
	TAManager* TAManager::instance = nullptr;

	TAManager::TAManager()
	{
		raceType = TARaceType::None;
		perfMeasurement = nullptr;
		trackManager = nullptr;

		// Obtain pointer to file manager
		fileManager = PFileManager::Get();
		
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

	PMManager* TAManager::PerformanceMeasurement() const
	{
		return perfMeasurement;
	}

	void TAManager::Init(tRmInfo* RaceManager)
	{
		// Retrieve load state
		PTrackLoadState loadState = fileManager->GetTrackLoadState();

		raceManager = RaceManager;
		trackManager = new PTrackManager(raceManager);

		// Initialize Race Mode for TORCS Adaptive
		if (loadState.LoadType() == PLoadType::TRACK)
			SetRaceType(TARaceType::Pregenerated);
		else if (loadState.LoadType() == PLoadType::CONFIG)
		{
			if (strcmp(raceManager->raceEngineInfo.name, ADAPTIVE_RACE) == 0)
				SetRaceType(TARaceType::Adaptive);
			else if (strcmp(raceManager->raceEngineInfo.name, PROCEDURAL_RACE) == 0)
				SetRaceType(TARaceType::Procedural);
			else
				SetRaceType(TARaceType::None);
		}

		// Initialize the HUD
		hud.Init(raceType == TARaceType::Adaptive);

		if (raceManager->raceEngineInfo.displayMode == RM_DISP_MODE_CONSOLE)
			raceOnConsole = true;
		else
			raceOnConsole = false;

		// Initialize the track
		InitTrack();
	}

	void TAManager::InitGraphics()
	{
		// Initialize Graphics
		PTrack* trInfo = trackManager->GetTrack(); // Retrieve track info

		if (!trInfo)
			taOut("Failed to initialize graphics component. Could not retrieve procedural track from procedural track manager. Ensure InitTrackManager is called before graphics initialization.\n");
		else
		{
			if (raceManager->_displayMode != RM_DISP_MODE_CONSOLE) // Load Initial 3D decsription if not displaying on console
			{
				raceManager->_reGraphicItf.PGrInit(trInfo);
				raceManager->_reGraphicItf.PGrAttach3DDesc(trInfo->GetTrackDesc()); // Attach 3D description to scene graph
			}
		}
	}

	void TAManager::InitTrack()
	{		
		trackManager->InitTrack();
	}

	void TAManager::InitPerfMeasurement(tCarElt* car, PMEvaluator* evaluator)
	{
		if (!perfMeasurement)
			perfMeasurement = PMManager::Get();

		perfMeasurement->Init(car, evaluator);
	}

	void TAManager::AddSegment(const PSeg& segment)
	{
		trackManager->AddSegment(segment);
	}

	void TAManager::UpdateTrack()
	{
		if (raceType == TARaceType::Adaptive)
		{
			currentSkillLevel = perfMeasurement->GetSkillEstimate();
			trackManager->Update(true, currentSkillLevel);
		}
		else
			trackManager->Update();
	}

	void TAManager::SetRaceType(const TARaceType& RaceType)
	{
		raceType = RaceType;
	}

	TARaceType TAManager::GetRaceType() const
	{
		return raceType;
	}

	PTrack* TAManager::GetTrack() const
	{
		return trackManager->GetTrack();
	}

	PTrackManager* TAManager::GetTrackManager() const
	{
		return trackManager;
	}

	bool TAManager::IsActive() const
	{
		if (raceType == TARaceType::None)
			return false;
		else
			return true;
	}

	bool TAManager::IsProcedural() const
	{
		if (raceType == TARaceType::Adaptive || raceType == TARaceType::Procedural)
			return true;
		else
			return false;
	}

	void TAManager::CheckIfFinished()
	{
		if (trackManager->CurrentLength() >= trackManager->TotalLength())
		{
			if (trackManager->IsCarFinished())
				raceManager->s->raceInfo.state = RM_RACE_ENDED;
		}
	}

	void TAManager::RaceEnd()
	{
		// Reset the load state at the end of a race
		fileManager->SetTrackLoadState(PTrackLoadState());
	}

	void TAManager::InitCars()
	{
		trackManager->InitCars();
	}

	void TAManager::DrawBoard()
	{
		if (perfMeasurement)
			hud.Render(perfMeasurement->GetSkillEstimate());
		else
			hud.Render(NULL_SKILL_LEVEL);
	}
}