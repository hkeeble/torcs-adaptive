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
	PTrackManager::PTrackManager()
	{
		raceManager = nullptr;
		track = nullptr;
		segFactory = nullptr;
		trackType = PTrackType::PROCEDURAL;
	}

	PTrackManager::PTrackManager(tRmInfo* RaceManager)
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

			// Obtain track length
			void* handle = GfParmReadFile((loadState.TrackPath() + loadState.TrackFileName()).c_str(), GFPARM_RMODE_STD);
			tdble trkLength = GfParmGetNum(handle, P_TRK_SECT_HDR, P_TRK_ATT_LENGTH, (char*)nullptr, 1000);

			// Initialize the procedural track object
			track = new PTrack(segs, trkLength, configpath, acpath, configname, acname, lopts);

			// Add a finish line
			track->AddFinishLine();

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
		segFactory = param.segFactory;
		previousSegType = param.previousSegType;
		previousCornerType = param.previousCornerType;
		trackType = param.trackType;
		carList = param.carList;
		nCars = param.nCars;
	}

	PTrackManager::~PTrackManager()
	{
		if (track)
			delete track;
	}

	void PTrackManager::AddSegment(const PSeg& segment)
	{
		pOut("Adding new Segment....\n");

		// Add Segment
		pOut("\tAdding segment to track.\n");
		track->AddSegment(segment);
		pOut("New segment added.\n");

		// Calculate new segment ranges
		if (segment.type != TR_STR)
		{
			PCornerType t;
			if (segment.type == TR_LFT)
				t = PCornerType::CTLeft;
			else
				t = PCornerType::CTRight;

			segFactory->UpdateRanges(track->GetEnd()->arc, t);
		}
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

				if (skillLevel != 0)
				{
					// Most difficult: smaller radius, larger arc.
					PSegmentRanges ranges = segFactory->ranges;
					
					// Seed random number generator by the current time
					 std::default_random_engine engine(std::chrono::system_clock::now().time_since_epoch().count());
					 std::normal_distribution<tdble> distribution(skillLevel, NORMAL_DSTRIBUTION);

					 // Produce a variant of the skill level using normal distribution, follow by clamping the value
					 tdble variant = distribution(engine);
					 if (variant > 1) variant = 1;
					 else if (variant < 0) variant = 0;

					// Ensure that weighting is never 0.0f, if skill level estimate is 1 use 0.1f, as this is the lowest possible weighting
					tdble radius = lerp(ranges.Radius().Min(), ranges.Radius().Max(), variant < 1.0f ? 1.0f - variant : 0.1f);

					// Generate arc weighted with skill level, based upon corner type
					tdble arc = 0.f;
					if (cType == PCornerType::CTRight)
						arc = lerp(ranges.RightArc().Min(), ranges.RightArc().Max(), variant);
					else
						arc = lerp(ranges.LeftArc().Min(), ranges.LeftArc().Max(), variant);

					// Create the segment with the segment factory
					curSeg = &segFactory->CreateSegCnr(track->state.curSegIndex, cType, radius, arc);
				}
				else
					curSeg = &segFactory->CreateRandomStr(track->state.curSegIndex);

				previousCornerType = cType;
			}
		}

		// Add segment
		AddSegment(*curSeg);
		previousSegType = curSeg->type;
	}

	void PTrackManager::Update(bool adaptive, float skillLevel)
	{
		// Prevent cars driving off the back of the track
		CorrectCars();

		if (trackType == PTrackType::PROCEDURAL)
		{
			// If the current leading car is a given distance from the end, generate a new segment
			if (LeadingCar()->pub.trkPos.seg->id + MAX_SEGS_FROM_END >= track->GetEnd()->id) // If a new segment needs to be generated
			{
				if (!adaptive) // Generate random segment if not adaptive
					AddSegment(segFactory->CreateRandomSeg(track->state.curSegIndex));
				else // Otherwise, generate segment taking into account skill level
					GenerateAdaptiveSegment(skillLevel);

				// Update graphical description
				if (raceManager->raceEngineInfo.displayMode == RM_DISP_MODE_NORMAL)
					UpdateGraphics();

				// Check if a finish line needs to be added
				CheckForFinishLine();
			}
		}
	}

	void PTrackManager::CheckForFinishLine()
	{
		// Check if a finish line needs to be added
		if (track->trk->length >= TotalLength())
		{
			int segBefore = track->trk->nseg;
			track->AddFinishLine();
			int segAfter = track->trk->nseg;

			// Append graphics if a finish line was added
			if (segAfter > segBefore)
			{
				if (raceManager->raceEngineInfo.displayMode == RM_DISP_MODE_NORMAL)
					UpdateGraphics();
			}
		}
	}

	tCarElt* PTrackManager::LeadingCar() const
	{
		tCarElt* leadingCar = carList;

		for (int i = 0; i < nCars; i++) // Loop through all cars
		{
			tCarElt* currentCar = &carList[i];

			// Find the leading car
			if (currentCar->pub.trkPos.seg->id > leadingCar->pub.trkPos.seg->id)
				leadingCar = currentCar;
			else if (currentCar->pub.trkPos.seg->id == leadingCar->pub.trkPos.seg->id)
			{
				if (currentCar->pub.trkPos.toStart < leadingCar->pub.trkPos.toStart)
					leadingCar = currentCar;
			}
		}

		return leadingCar;
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

	PTrack* PTrackManager::GetTrack() const
	{
		return track;
	}

	bool PTrackManager::IsCarFinished()
	{
		for (int i = 0; i < nCars; i++)
		{
			tCarElt* curCar = &carList[i];

			// Calculate the distance covered by this car
			tdble distCovered = curCar->pub.trkPos.seg->lgfromstart;

			if (distCovered >= TotalLength())
				return true;
		}

		return false;
	}

	void PTrackManager::CorrectCars()
	{
		for (int i = 0; i < nCars; i++)
		{
			tCarElt* curCar = &carList[i];

			// If current car on first segment, prevent from leaving segment
			if (curCar->pub.trkPos.seg->id == 0)
			{
				if (curCar->pub.trkPos.toStart < 10)
				{
					curCar->pub.trkPos.toStart = 10;
					RtTrackLocal2Global(&(curCar->_trkPos), &(curCar->_pos_X), &(curCar->_pos_Y), TR_TORIGHT);
					raceManager->_reSimItf.config(curCar, raceManager);
				}
			}
		}
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
		fManager->OutputTrack(name, configDir, raceManager->track->name, track);
	}

	void PTrackManager::InitCars()
	{
		// Get pointer to car list
		carList = raceManager->carList;

		// Retrieve number of cars in race
		nCars = raceManager->s->raceInfo.ncars;

		// Start segment
		tTrackSeg* startSeg = track->GetStart();

		// Array of positions
		tTrkLocPos* positions = new tTrkLocPos[nCars];
		for (int i = 0; i < nCars; i++)
			positions[i] = tTrkLocPos();

		// Spacing between cars
		tdble carXSpacing = (startSeg->width / 2) - 3; // Assuming 3 is around average car width
		tdble carYSpacing = 20;

		/* Starting grid positons are calculated regardless of pole position as follows:
		    Y
			|---------|
			| P1  P2  |
			| P3  P4  |
			| P5  P6  |
			| P7  P8  |
			| P9  P10 | 
			|---------| - X
		*/

		// Calculate position grid here
		if (nCars > 1)
		{
			int row = 1;
			for (int i = 0; i < nCars; i++) // Grid Y
			{
				tTrkLocPos* curPos = &positions[i];

				curPos->seg = startSeg;
				curPos->toStart = startSeg->length - (carYSpacing * row);

				if (i % 2 > 0) // RIGHT POS
					curPos->toMiddle = carXSpacing;
				else if (i % 2 == 0) // LEFT POS
				{
					curPos->toMiddle = -carXSpacing;
					row++;
				}
			}
		}
		else
		{
			positions[0].seg = startSeg;
			positions[0].toStart = startSeg->length - carYSpacing;
			positions[0].toMiddle = carXSpacing;
		}

		// Assign car positions
		for (int i = 0; i < nCars; i++)
		{
			// Get the car
			tCarElt* curCar = &carList[i];

			// Assign the car the correct position
			curCar->pub.trkPos = positions[i];

			// Convert the local position to a global one, and assign to car's actual global position
			RtTrackLocal2Global(&(curCar->_trkPos), &(curCar->_pos_X), &(curCar->_pos_Y), TR_TOMIDDLE);
		}

		// Set up cars in physics sim
		for (int i = 0; i < nCars; i++)
		{
			tCarElt* curCar = &carList[i];
			raceManager->_reSimItf.config(curCar, raceManager);
		}

		delete[] positions;
	}
}