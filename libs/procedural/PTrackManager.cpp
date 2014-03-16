/*
	File: ProceduralTrackManager.cpp
	Author: Henri Keeble
	Desc: Defines a singleton class to manage a procedural track.
*/
#include "PTrackManager.h"
#include "PFileManager.h"
#include "taMath\taMath.h"
#include <cmath>

namespace procedural
{
	PTrackManager::PTrackManager() : MAX_DIST_FROM_END(5)
	{
		raceManager = nullptr;
		track = nullptr;
		segFactory = nullptr;
		trackType = PTrackType::PROCEDURAL;
	}

	PTrackManager::PTrackManager(tRmInfo* RaceManager) : MAX_DIST_FROM_END(3)
	{
		pOut("Initializing procedural track manager...\n");
		raceManager = RaceManager; // Save pointer to race manager
	}

	void PTrackManager::InitTrack()
	{
		// Retrieve the load state from the file manager
		PTrackLoadState loadState = PFileManager::Get()->GetTrackLoadState();

		// If no config is set, then set to the first config in the directory
		if (!loadState.ConfigName().compare(NO_CONFIG_SET))
		{
			PFileManager* fManager = PFileManager::Get();
			std::vector<std::string> configs = fManager->DirectoriesInDirectory(std::string(fManager->GetCurrentDir()) + "tracks\\procedural\\");
			loadState.SetConfiguration(configs[0], "tracks/procedural/" + configs[0] + "/");
		}

		pOut("Track manager obtaining pointer to segment factory...\n");
		segFactory = PSegFactory::GetInstance(); // Obtain pointer to segment factory

		pOut("Initializing segment factory...\n");
		segFactory->SetChances(45.f, 65.f);

		// Set neccesary paths
		std::string modeldir("tracks/procedural/" + loadState.ConfigName() + "/");
		std::string texturedir("data/textures");

		pOut("Setting Track 3D Description Loader Options...\n");
		ssgLoaderOptions* lopts = new ssgLoaderOptions();
		lopts->setModelDir(modeldir.c_str());
		lopts->setTextureDir(texturedir.c_str());

		if (loadState.LoadType() == PLoadType::CONFIG) // Default TORCS Adaptive track initialization - initializes a configuration for a procedural track
		{
			pOut("Initialize procedural track configuration...\n");

			pOut("Setting track file path and file names...\n");
			std::string acname(loadState.ConfigName() + ".ac");
			std::string acpath("tracks/procedural/" + loadState.ConfigName() + "/");
			std::string configpath(acpath);
			std::string configname(loadState.ConfigFileName());

			// Initialize the procedural track.
			pOut("Initializing procedural track structure...\n");
			track = new PTrack(loadState.Length(), configpath, acpath, configname, acname, lopts);

			// Point the racemanager to the procedural track
			pOut("Setting racemanager track to procedural track...\n");
			raceManager->track = track->trk;

			previousSegType = track->GetEnd()->type;

			// Set track type
			trackType = PTrackType::PROCEDURAL;
		}
		else if (loadState.LoadType() == PLoadType::TRACK) // Initialization if a pre-generated track is to be loaded
		{
			// Read in the segments
			std::vector<PSeg> segs = PFileManager::Get()->ReadTrackSegments(loadState.TrackPath() + loadState.TrackFileName());

			// Construct paths and names
			std::string acname(loadState.TrackName() + ".ac");
			std::string acpath("tracks/procedural/" + loadState.ConfigName() + "/previousTracks/" + loadState.TrackName() + "/");
			std::string configpath("tracks/procedural/" + loadState.ConfigName() + "/");
			std::string configname(loadState.ConfigFileName());

			// Initialize the procedural track object
			track = new PTrack(segs, configpath, acpath, configname, acname, lopts);

			// Point the racemanager to the procedural track
			raceManager->track = track->trk;

			// Set the track's name
			raceManager->track->name = new char[loadState.TrackName().length()];
			strcpy((char*)raceManager->track->name, loadState.TrackName().c_str());
			strcpy(raceManager->track->internalname, loadState.TrackName().c_str());

			// Set track type
			trackType = PTrackType::PREGENERATED;
		}

		// Create a random previous corner type to start with
		int c = segFactory->CreateRandomCnr(0).type;
		if (c == TR_LFT)
			previousCornerType = PCornerType::CTLeft;
		else
			previousCornerType = PCornerType::CTRight;
	}

	PTrackManager::PTrackManager(const PTrackManager& param) : MAX_DIST_FROM_END(3)
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
		track->AddSegment(segment);
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
				// Ensure corner is a different direction to the last
				PCornerType cType = PCornerType::CTLeft;
				if (previousCornerType == PCornerType::CTLeft)
					cType = PCornerType::CTRight;

				// Most difficult: smaller radius, larger arc.
				PSegmentRanges ranges = segFactory->ranges;
				tdble radius = lerp(ranges.Radius().Min(), ranges.Radius().Max(), 1.0f - skillLevel);
				tdble arc = lerp(ranges.Arc().Min(), ranges.Arc().Max(), skillLevel);

				curSeg = &segFactory->CreateSegCnr(track->state.curSegIndex, cType, radius, arc);

				previousCornerType = cType;
			}
		}

		AddSegment(*curSeg);
		previousSegType = curSeg->type;
	}

	void PTrackManager::Update(bool adaptive, float skillLevel)
	{
		// Update data on the car
		carData.Update();

		if (trackType == PTrackType::PROCEDURAL)
		{
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
	}

	void PTrackManager::UpdateGraphics()
	{
		// Update Graphics Module
		pOut("\tUpdating Graphics Module.\n");
		raceManager->_reGraphicItf.PGrDetach3DDesc(track->GetTrackDesc()); // Detach existing description from scene graph

		pOut("\tGenerating new 3D Description.\n");

		// Update the AC File
		UpdateACFile();

		// Append existing 3D Description
		raceManager->_reGraphicItf.PGrAppend3DDesc(track);

		// Reattach 3D description
		raceManager->_reGraphicItf.PGrAttach3DDesc(track->GetTrackDesc());
	}

	void PTrackManager::UpdateACFile()
	{
		track->UpdateACFile();
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

	void PTrackManager::OutputCurrentTrack(std::string name)
	{
		// Obtain a pointer to the file manager
		PFileManager* fManager = PFileManager::Get();

		// Declare the strings to be used
		std::string fileName;
		std::string configDir;

		// Get executable's directory
		char* executableDir = fManager->GetCurrentDir();

		// Build track config directory path
		configDir = std::string(executableDir) + track->GetConfigPath();

		// Create name of file from file count
		fileName = name + ".xml"; // Append with track desc extension

		// Write out the track
		fManager->OutputTrack(name, configDir, raceManager->track->name, track->trk);
	}
}