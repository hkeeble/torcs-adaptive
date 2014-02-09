/*
	File: ProceduralTrackManager.cpp
	Author: Henri Keeble
	Desc: Defines a singleton class to manage a procedural track.
*/
#include "PTrackManager.h"
#include "trackgen\trackgen.h"

namespace procedural
{
	PTrackManager::PTrackManager()
	{
		raceManager = nullptr;
		track = nullptr;
		segFactory = nullptr;
	}

	PTrackManager::PTrackManager(std::string trackName, tdble trackLength, tRmInfo* RaceManager)
	{
		taOut("Initializing procedural track manager...\n");

		std::string acname, xmlname, filePath, modeldir, texturedir;

		taOut("Setting track file path and file names...\n");
		acname = trackName + ".ac";
		xmlname = trackName + ".xml";
		filePath = "tracks/adaptive/" + trackName + "/";
		modeldir = "tracks\\adaptive\\" + trackName + "\\";
		texturedir = "data\\textures";

		taOut("Setting Track 3D Description Loader Options...\n");
		ssgLoaderOptions* lopts = new ssgLoaderOptions();
		lopts->setModelDir(modeldir.c_str());
		lopts->setTextureDir(texturedir.c_str());

		taOut("Track manager obtaining pointers to segment factory and racemanager...\n");
		segFactory = PSegFactory::GetInstance(); // Obtain pointer to segment factory
		raceManager = RaceManager; // Save pointer to race manager

		// Initialize the procedural track, and point racemanager to the procedural track
		taOut("Initializing procedural track structure and TORCS track structure...\n");
		track = raceManager->_reTrackItf.PTrackInit(trackLength, (char*)acname.c_str(), (char*)xmlname.c_str(), (char*)filePath.c_str(),
			lopts, raceManager->raceEngineInfo.displayMode == RM_DISP_MODE_CONSOLE);

		taOut("Setting racemanager track to procedural track...\n");
		raceManager->track = track->trackCache;
	}

	PTrackManager::PTrackManager(const PTrackManager& param)
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
	}

	PTrackManager::~PTrackManager()
	{
		if (track)
			delete track;
	}

	void PTrackManager::Init(tCarElt* car)
	{
		carData = PCarData(car);

		// Seed random generator
		srand(time(0));

		generateSegment = false;
		updateAC = false;

		previousCornerType = PCornerType::CTRight;
	}

	void PTrackManager::AddSegment(const PSeg& segment)
	{
		taOut("Adding new Segment....\n");

		// Add Segment
		taOut("\tAdding segment to track.\n");
		raceManager->_reTrackItf.PAddSegment(segment, track);
		taOut("New segment added.\n");
	}

	/* Potentially could be used to append AC file where neccesary */
	void PTrackManager::UpdateACFile()
	{
		// Update Graphics Module
		taOut("\tUpdating Graphics Module.\n");
		raceManager->_reGraphicItf.PDetach3DDesc(track->GetTrackDesc()); // Detach existing description from scene graph

		taOut("\tGenerating new 3D Description.\n");

		// Update the AC File (could maybe move this from external library into this class, for the sake of clarity.)
		raceManager->_reTrackItf.PUpdateACFile(track);

		// Append existing 3D Description
		raceManager->_reGraphicItf.PAppend3DDesc(track);

		// Reattach 3D description
		raceManager->_reGraphicItf.PAttach3DDesc(track->GetTrackDesc());
	}

	void PTrackManager::ManageCache()
	{
		//// If car is on last segment and travelling forward, remove one at the start
		//if (carData.CurrentSeg()->id == track->GetEnd()->id)
		//{
		//	if (carData.DirOfTravel() == DirectionOfTravel::FORWARD)
		//	{
		//		track->RemoveSegAtStart();
		//		updateAC = true;
		//	}
		//}

		//// Else if car travelling backwards and on first segment, remove a segment at the end. If car on first segment generated, do nothing (THIS NEEDS ANOTHER SOLUTION!)
		//else if (carData.CurrentSeg()->id == track->GetStart()->id && carData.CurrentSeg()->id != 0)
		//{
		//	if (carData.DirOfTravel() == DirectionOfTravel::BACKWARD)
		//	{
		//		track->RemoveSegAtEnd();
		//		updateAC = true;
		//	}
		//}

		// Determine if another segment needs to be added to the cache and/or track
		//if (track->SEG_MEMORY_SIZE > track->trackCache->nseg)
		//{
		//	switch (carData.DirOfTravel())
		//	{
		//	case DirectionOfTravel::FORWARD:
		//		// Determine if new segment needs to be generated
		//		if (track->trackCache->seg->id == track->segs.End()->id)
		//			generateSegment = true;
		//		// else
		//			// track->AddSegmentAtEnd(); // Add existing segment

		//		updateAC = true;
		//		break;

		//	//case DirectionOfTravel::BACKWARD:
		//	//	if (carData.CurrentSeg()->id > 0) // Only add new segment if player is not at the start
		//	//	{
		//	//		track->AddSegmentAtStart();
		//	//		updateAC = true;
		//	//	}
		//	//	break;
		//	default:
		//		taOut("Unexpected direction of travel!\n");
		//		break;
		//	}
		//}

		if (carData.CurrentSeg()->id == track->GetEnd()->id)
		{
			generateSegment = true;
			updateAC = true;
		}
	}

	void PTrackManager::ManageSegmentGeneration(float skillLevel)
	{
		if (generateSegment)
		{
			generateSegment = false;

			/* If skillLevel is -1, generate random segment */
			if (skillLevel == -1.f)
			{
				PSegType type = RandomSegmentType();

				if (type == PSegType::Straight)
					AddSegment(segFactory->CreateSegStr(track->state.curSegIndex, RandBetween(ranges.Length().Min(), ranges.Length().Max()))); // Generate new segment
				else
				{
					float radius = RandBetween(ranges.Radius().Min(), ranges.Radius().Max());
					float arc = RandBetween(ranges.Arc().Min(), ranges.Arc().Max());
					PCornerType cType;

					if (previousCornerType == CTLeft)
						cType = CTRight;
					else
						cType = CTLeft;

					AddSegment(segFactory->CreateSegCnr(track->state.curSegIndex, cType, radius, 0.f, 0.f, arc));
					previousCornerType = cType;
				}
			}
			else // otherwise, take into account skill level
			{
				AddSegment(segFactory->CreateSegStr(track->state.curSegIndex, 200.f)); // PLACEHOLDER
			}
		}
	}

	PSegType PTrackManager::RandomSegmentType()
	{
		PSegType type;

		// Determine highest chance value
		int highVal = PSegType::Straight;
		if (straightChance < cornerChance)
			highVal = PSegType::Corner;

		float val = rand() % 100; // Generate random value

		if (highVal == PSegType::Corner)
		{
			if (val >= cornerChance)
				type = PSegType::Corner;
			else
				type = PSegType::Straight;
		}
		else if (highVal == PSegType::Straight)
		{
			if (val >= straightChance)
				type = PSegType::Straight;
			else
				type = PSegType::Corner;
		}

		return type;
	}

	void PTrackManager::Update(float skillLevel)
	{
		// Update data on the car
		carData.Update();

		// Manage the current cache of segments
		ManageCache();

		// Manage segment generation, if new segments are needed
		if (track->segs.Length() < track->TotalLength())
			ManageSegmentGeneration(skillLevel);

		UpdateGraphics();
	}

	void PTrackManager::UpdateGraphics()
	{
		// Update AC File if necessary
		if (updateAC)
		{
			updateAC = false;
			UpdateACFile();
		}
	}

	tdble PTrackManager::PercentDistanceToEnd(tdble segLength, tdble distToStart)
	{
		tdble percent = segLength / 100; // Equal to one percent of the segment
		return 100 - (distToStart / percent); // Calculate percent distance from start, take from 100 to find percent distance to end
	}

	float PTrackManager::RandBetween(float min, float max)
	{
		if (min < max)
			return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
		else
			taOut("PTrackManager: Min cannot be less thatn Max!\n");
	}

	tdble PTrackManager::CurrentLength() const
	{
		return track->segs.Length();
	}

	tdble PTrackManager::TotalLength() const
	{
		return track->TotalLength();
	}

	PCarData PTrackManager::CarData() const
	{
		return carData;
	}

	PTrack* PTrackManager::GetTrack() const
	{
		return track;
	}

	bool PTrackManager::CarOnLastSegment()
	{
		return (carData.CurrentSeg()->id == track->segs.End()->id);
	}

	tTrack* PTrackManager::BuildTrack()
	{
		return track->BuildTrack();
	}
}