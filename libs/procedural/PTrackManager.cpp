/*
	File: ProceduralTrackManager.cpp
	Author: Henri Keeble
	Desc: Defines a singleton class to manage a procedural track.
*/
#include "PTrackManager.h"
#include "trackgen\trackgen.h"

#define taOut(out) printf("ta >> " out)

namespace procedural
{
	PTrackManager::PTrackManager()
	{

	}

	PTrackManager::~PTrackManager()
	{

	}

	void PTrackManager::Init(tCarElt* car, tRmInfo* RaceManager, PTrack* Track)
	{
		carData = PCarData(car);
		raceManager = RaceManager;
		track = Track;
		segFactory = PSegFactory::GetInstance(); // Obtain pointer to segment factory

		// Seed random generator
		srand(time(0));

		generateSegment = false;
		updateAC = false;

		previousCornerType = PCornerType::CTRight;
	}

	void PTrackManager::AddSegment(const PSeg& segment)
	{
		taOut("Adding new Segment....\n");

		// Obtain pointers to neccesary data
		PTrack*			  atrack		= raceManager->_reTrackItf.PGetTrackInfo();
		const char *const acName		= atrack->GetACName();
		const char *const acNameAndPath = atrack->GetACPathAndName();

		// Add Segment
		taOut("\tAdding segment to track.\n");
		raceManager->_reTrackItf.PAddSegment(segment, atrack);
		taOut("New segment added.\n");
	}
	
	/* Potentially could be used to append AC file where neccesary */
	void PTrackManager::UpdateACFile()
	{
		// Update Graphics Module
		taOut("\tUpdating Graphics Module.\n");
		raceManager->_reGraphicItf.pDetach3DDesc(track->GetTrackDesc()); // Detach existing description from scene graph

		taOut("\tGenerating new 3D Description.\n");
		GenerateTrack(raceManager->track, raceManager->track->params, (char*)track->GetACPathAndName(), NULL, NULL, NULL, 0); // Generate new 3d desc
		track->SetTrackDesc(raceManager->_reGraphicItf.pLoad3DDesc(track->GetACName(), (ssgLoaderOptions*)track->GetLoaderOptions()));
		raceManager->_reGraphicItf.pAttach3DDesc(track->GetTrackDesc());
	}

	void PTrackManager::ManageCache()
	{
		// If car is on last segment and travelling forward, remove one at the start
		if (carData.CurrentSeg()->id == track->GetEnd()->id)
		{
			if (carData.DirOfTravel() == DirectionOfTravel::FORWARD)
			{
				track->RemoveSegAtStart();
				updateAC = true;
			}
		}

		// Else if car travelling backwards and on first segment, remove a segment at the end. If car on first segment generated, do nothing (THIS NEEDS ANOTHER SOLUTION!)
		else if (carData.CurrentSeg()->id == track->GetStart()->id && carData.CurrentSeg()->id != 0)
		{
			if (carData.DirOfTravel() == DirectionOfTravel::BACKWARD)
			{
				track->RemoveSegAtEnd();
				updateAC = true;
			}
		}

		// Determine if another segment needs to be added to the cache and/or track
		if (track->SEG_MEMORY_SIZE > track->trackCache->nseg)
		{
			switch (carData.DirOfTravel())
			{
			case DirectionOfTravel::FORWARD:
				// Determine if new segment needs to be generated
				if (track->trackCache->seg->id == track->segs.End()->id)
					generateSegment = true;
				else
					track->AddSegmentAtEnd(); // Add existing segment

				updateAC = true;
				break;

			case DirectionOfTravel::BACKWARD:
				if (carData.CurrentSeg()->id > 0) // Only add new segment if player is not at the start
				{
					track->AddSegmentAtStart();
					updateAC = true;
				}
				break;
			default:
				taOut("Unexpected direction of travel!\n");
				break;
			}
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

	void PTrackManager::ManageACFile()
	{
		// Update AC File if necessary
		if (updateAC)
		{
			updateAC = false;
			UpdateACFile();
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

		// Manage segment generation
		ManageSegmentGeneration(skillLevel);

		// Manage the track's AC file
		ManageACFile();
	}

	tdble PTrackManager::PercentDistanceToEnd(tdble segLength, tdble distToStart)
	{
		tdble percent = segLength / 100; // Equal to one percent of the segment
		return 100 - (distToStart / percent); // Calculate percent distance from start, take from 100 to find percent distance to end
	}

	float PTrackManager::RandBetween(float min, float max)
	{
		if (min < max)
			return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max - min)));
		else
			taOut("PTrackManager: Min cannot be less thatn Max!\n");
	}
}