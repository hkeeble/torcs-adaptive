/*
	File: ProceduralTrackManager.cpp
	Author: Henri Keeble
	Desc: Defines a singleton class to manage a procedural track.
*/
#include "PTrackManager.h"
#include "trackgen\trackgen.h"

namespace procedural
{
	PTrackManager::PTrackManager() : MAX_DIST_FROM_END(5)
	{
		raceManager = nullptr;
		track = nullptr;
		segFactory = nullptr;
	}

	PTrackManager::PTrackManager(std::string trackName, tdble trackLength, tRmInfo* RaceManager) : MAX_DIST_FROM_END(5)
	{
		pOut("Initializing procedural track manager...\n");

		std::string acname, xmlname, filePath, modeldir, texturedir;

		pOut("Setting track file path and file names...\n");
		acname = trackName + ".ac";
		xmlname = trackName + ".xml";
		filePath = "tracks/procedural/" + trackName + "/";
		modeldir = "tracks\\adaptive\\" + trackName + "\\";
		texturedir = "data\\textures";

		pOut("Setting Track 3D Description Loader Options...\n");
		ssgLoaderOptions* lopts = new ssgLoaderOptions();
		lopts->setModelDir(modeldir.c_str());
		lopts->setTextureDir(texturedir.c_str());

		pOut("Track manager obtaining pointers to segment factory and racemanager...\n");
		segFactory = PSegFactory::GetInstance(); // Obtain pointer to segment factory
		raceManager = RaceManager; // Save pointer to race manager

		pOut("Initializing segment factory...\n");
		segFactory->SetChances(45.f, 65.f);

		// Initialize the procedural track, and point racemanager to the procedural track
		pOut("Initializing procedural track structure and TORCS track structure...\n");
		track = raceManager->_reTrackItf.PTrackInit(trackLength, (char*)acname.c_str(), (char*)xmlname.c_str(), (char*)filePath.c_str(),
			lopts, raceManager->raceEngineInfo.displayMode == RM_DISP_MODE_CONSOLE);

		pOut("Setting racemanager track to procedural track...\n");
		raceManager->track = track->trk;

		previousSegType = track->GetEnd()->type;
	}

	PTrackManager::PTrackManager(const PTrackManager& param) : MAX_DIST_FROM_END(5)
	{
		cpy(param);
	}

	PTrackManager& PTrackManager::operator=(const PTrackManager& param)
	{
		if (&param == this)
			return *this;
		else
		{
			if (track)
				delete track;

			cpy(param);

			return *this;
		}
	}

	void PTrackManager::cpy(const PTrackManager& param)
	{
		track = new PTrack(*param.GetTrack());
		raceManager = param.raceManager;
		previousSegType = param.previousSegType;
	}

	PTrackManager::~PTrackManager()
	{
		if (track)
			delete track;
	}

	void PTrackManager::Init(tCarElt* car)
	{
		carData = CarData(car);
	}

	void PTrackManager::AddSegment(const PSeg& segment)
	{
		pOut("Adding new Segment....\n");

		// Add Segment
		pOut("\tAdding segment to track.\n");
		raceManager->_reTrackItf.PAddSegment(segment, track);
		pOut("New segment added.\n");
	}

	void PTrackManager::GenerateAdaptiveSegment(float skillLevel)
	{
		PSeg* curSeg = nullptr;

		if (previousSegType != TR_STR) // If the previous segment was a corner, ensure another corner is not generated
			curSeg = &segFactory->CreateRandomStr(track->state.curSegIndex);
		else
		{
			/* If skillLevel is -1, generate random segment */
			if (skillLevel == -1.f)
				curSeg = &segFactory->CreateRandomSeg(track->state.curSegIndex);
			else // otherwise, take into account skill level
			{
				curSeg = &segFactory->CreateRandomSeg(track->state.curSegIndex); // PLACEHOLDER
			}
		}

		AddSegment(*curSeg);
		previousSegType = curSeg->type;
	}

	void PTrackManager::Update(bool adaptive, float skillLevel)
	{
		// Update data on the car
		carData.Update();

		// Manage new segment generation
		if (track->trk->length < track->TotalLength())
		{
			if (carData.CurrentSeg()->id + MAX_DIST_FROM_END >= track->GetEnd()->id) // If a new segment needs to be generated
			{
				if (!adaptive)
					AddSegment(segFactory->CreateRandomSeg(track->state.curSegIndex));
				else
					GenerateAdaptiveSegment(skillLevel);

				// Update graphical description
				if (raceManager->raceEngineInfo.displayMode == RM_DISP_MODE_NORMAL)
					UpdateGraphics();
			}
		}
	}

	void PTrackManager::UpdateGraphics()
	{
		// Update Graphics Module
		pOut("\tUpdating Graphics Module.\n");
		raceManager->_reGraphicItf.PGrDetach3DDesc(track->GetTrackDesc()); // Detach existing description from scene graph

		pOut("\tGenerating new 3D Description.\n");

		// Update the AC File (could maybe move this from external library into this class, for the sake of clarity.)
		raceManager->_reTrackItf.PUpdateACFile(track);

		// Append existing 3D Description
		raceManager->_reGraphicItf.PGrAppend3DDesc(track);

		// Reattach 3D description
		raceManager->_reGraphicItf.PGrAttach3DDesc(track->GetTrackDesc());
	}

	tdble PTrackManager::PercentDistanceToEnd(tdble segLength, tdble distToStart)
	{
		tdble percent = segLength / 100; // Equal to one percent of the segment
		return 100 - (distToStart / percent); // Calculate percent distance from start, take from 100 to find percent distance to end
	}

	tdble PTrackManager::CurrentLength() const
	{
		return track->trk->length;
	}

	tdble PTrackManager::TotalLength() const
	{
		return track->TotalLength();
	}

	CarData PTrackManager::GetCarData() const
	{
		return carData;
	}

	PTrack* PTrackManager::GetTrack() const
	{
		return track;
	}

	bool PTrackManager::CarOnLastSegment()
	{
		return (carData.CurrentSeg()->id == track->GetEnd()->id);
	}
}