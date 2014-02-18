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

	PMManager* TAManager::PerformanceMeasurement() const
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

	void TAManager::InitTrack(std::string trackName)
	{
		if (trackManager)
			delete trackManager;

		if (!raceManager)
			taOut("Failed to initialize procedural track in TAManager. Ensure Init is called before InitTrack.\n");
		else
			trackManager = new PTrackManager(trackName, TA_TR_LENGTH, raceManager);
	}


	void TAManager::InitTrkManager(tCarElt* car)
	{
		if (!trackManager)
			taOut("Failed to initialize track manager. Ensure InitTrack is called before InitTrkManager.\n");
		else
		{
			trackManager->Init(car);
			this->car = car;
		}
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
			trackManager->Update(currentSkillLevel);
			currentSkillLevel = perfMeasurement->GetSkillEstimate();
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
			if (trackManager->CarOnLastSegment())
				raceManager->s->raceInfo.state = RM_RACE_ENDED;
		}
	}

	void TAManager::RaceEnd()
	{
		/* Plot the track for reading in gnuplot */
		TrackDesc desc = TrackDesc(trackManager->GetTrack()->trk);
		desc.plot((char*)(std::string((char*)trackManager->GetTrack()->GetFilePath()) + std::string("procTrackPlot.dat")).c_str());
	}

	void TAManager::InitCarPos()
	{
		tTrackSeg* startSeg = trackManager->GetTrack()->trk->seg;

		tCarElt* theCar = &raceManager->carList[0];

		// Create a local position in the center of the first segment
		theCar->_trkPos.seg = startSeg;
		theCar->_trkPos.type = startSeg->type;

		if (startSeg->type == TR_STR)
		{
			theCar->_trkPos.toStart = (startSeg->length / 2) - (theCar->info.dimension.z / 2);
			theCar->_trkPos.toLeft = (startSeg->width / 2) - (theCar->info.dimension.x / 2);
			theCar->_trkPos.toRight = theCar->_trkPos.toLeft;
		}
		else if (startSeg->type == TR_LFT || startSeg->type == TR_RGT)
		{
			theCar->_trkPos.toStart = ((startSeg->length / 2) - (theCar->info.dimension.z / 2)) / startSeg->radius;
			theCar->_trkPos.toLeft = (startSeg->width / 2) - (theCar->info.dimension.x / 2);
			theCar->_trkPos.toRight = theCar->_trkPos.toLeft;

			// Rotate car by yaw according to corner direction and normalize between 0 and 2PI
			theCar->_yaw = startSeg->angle[TR_ZS] + (startSeg->type == TR_LFT ? theCar->_trkPos.toStart : -theCar->_trkPos.toStart);
			NORM0_2PI(theCar->_yaw);
		}
		else
			taOut("Error assinging car initial position. Initial segment is of unrecognized type.\n");

		// Convert the local position to a global one, and assign to car's actual global position
		RtTrackLocal2Global(&(theCar->_trkPos), &(theCar->_pos_X), &(theCar->_pos_Y), TR_TORIGHT);

		// Set up car in physics sim
		raceManager->_reSimItf.config(theCar, raceManager);
	}
}