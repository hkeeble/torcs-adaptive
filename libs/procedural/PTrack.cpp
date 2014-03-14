/*
	File: PTrack.cpp
	Author: Henri Keeble
	Desc: Defines a class used to represent a procedural track in TORCS. Encapsulates all data required to manage and update a procedural track.
*/
#include "PTrack.h"
#include "PTrackState.h"
#include "pDefs.h"
#include "trackgen\trackgen.h"
#include <fstream>

namespace procedural
{
	// Track Info Definitions
	PTrack::PTrack(tdble totalLength, string configpath, string acpath, string configname, string acname, ssgLoaderOptions* loaderoptions)
	{
		// Initialize track state
		state = PTrackState();
		root = nullptr;
		this->totalLength = totalLength;

		// Assign paths and file names
		this->configPath = configpath;
		this->configName = configname;
		this->acPath = acpath;
		this->acName = acname;
		
		// Create a temporary AC file name.
		string tmp(acName);
		int lngth = tmp.length();
		tmp[lngth - 3] = 't';
		tmp[lngth - 2] = 'm';
		tmp[lngth - 1] = 'p';
		tmp.append(".ac");
		tempACName = tmp;

		// Initialize loader state
		ssgState = new PSSGState();
		ssgState->SetLoaderOptions(new ssgLoaderOptions(*loaderoptions));

		// Build the TORCS track structure
		trk = BuildTrack(GetConfigPathAndName().c_str());

		// Add initial segment
		AddSegment(PSegFactory::GetInstance()->CreateRandomStr(0));

		// Generate an initial 3D description
		GenerateTrack(trk, trk->params, const_cast<char*>(GetACPathAndName().c_str()), nullptr, 0, 0, 0);
	}

	PTrack::PTrack(std::vector<PSeg> segs, string configpath, string acpath, string configname, string acname, ssgLoaderOptions* loaderoptions)
	{
		// Assign paths and file names
		this->configPath = configpath;
		this->configName = configname;
		this->acPath = acpath;
		this->acName = acname;
		
		// Build the track
		trk = BuildTrack(GetConfigPathAndName().c_str());

		if (trk)
		{
			// Initialize state and root
			state = PTrackState();
			root = nullptr;

			// Add all segments to the track
			for (int i = 0; i < segs.size(); i++)
				AddSegment(segs[i]);

			// Generate a 3D description for the track
			GenerateTrack(trk, trk->params, const_cast<char*>(GetACPathAndName().c_str()), nullptr, 0, 0, 0);

			// Assign total length
			totalLength = trk->length;

			// Initialize SSG State (this should stay static for a pregenerated track)
			ssgState = new PSSGState();
			ssgState->SetLoaderOptions(new ssgLoaderOptions(*loaderoptions));

			// Set start and end segments
			SetStart(trk->seg->next);
			SetEnd(trk->seg);
		}
		else
			pOut("Error! Failed to build track structure from configuration file!\n");
	}

	PTrack::PTrack(const PTrack& param)
	{
		cpy(param);
	}

	PTrack& PTrack::operator=(const PTrack& param)
	{
		if (this == &param)
			return *this;
		else
		{
			cpy(param);

			return *this;
		}
	}

	void PTrack::cpy(const PTrack& param)
	{
		state = param.state;
		acName = param.acName;
		acPath = param.acPath;
		configName = param.configName;
		configPath = param.configPath;

		if (param.root)
			root = new tTrackSeg(*param.root);
		else
			root = nullptr;

		if (param.trk)
			trk = new tTrack(*param.trk);
		else
			trk = nullptr;

		if (param.start)
		{
			tTrackSeg* curSeg = trk->seg;
			while (curSeg->id != param.start->id)
				curSeg = curSeg->prev;
			start = curSeg;
		}
		else
			start = nullptr;

		if (param.end)
		{
			tTrackSeg* curSeg = trk->seg;
			while (curSeg->id != param.end->id)
				curSeg = curSeg->prev;
			start = curSeg;
		}
		else
			end = nullptr;

		if (param.ssgState)
			ssgState = param.ssgState;
		else
			ssgState = nullptr;
	}

	/* -- ACCESSOR DEFINITIONS  -- */

	PSSGState* PTrack::GetSSGState()						 { return ssgState; }
	EntityDesc* PTrack::GetTrackDesc() const				 { return ssgState->GetDesc(); }
	const ssgLoaderOptions *const PTrack::GetLoaderOptions() { return ssgState->GetLoaderOptions(); }
	
	const string& PTrack::GetACName()			 { return acName; }
	const string& PTrack::GetACPath()			 { return acPath; }
	const string& PTrack::GetTempACName()		 { return tempACName; }
	const string& PTrack::GetTempACPath()		 { return acPath; }
	const string& PTrack::GetConfigName()	     { return configName; }
	const string& PTrack::GetConfigPath()		 { return configPath; }
	const string PTrack::GetTempACPathAndName()  { return acPath + tempACName; }
	const string PTrack::GetACPathAndName()		 { return acPath + acName; }
	const string PTrack::GetConfigPathAndName()	 { return configPath + configName; }

	void PTrack::UpdateACFile()
	{
		tTrack* trackTmp = BuildTrack(GetConfigPathAndName().c_str());

		// Initialize a single segment as the last segment in the given track
		trackTmp->seg = new tTrackSeg(*GetEnd());

		// Break links to actual track
		trackTmp->seg->prev = trackTmp->seg;
		trackTmp->seg->next = trackTmp->seg;

		// Manually set some parameters used by track generator
		trackTmp->nseg = 1;
		trackTmp->length = trackTmp->seg->length;

		// Generate 3D description in temporary file
		GenerateTrack(trackTmp, trackTmp->params, const_cast<char*>(GetTempACPathAndName().c_str()), nullptr, 0, 0, 0);

		// Update track's AC file
		AppendACFile(trackTmp->seg->id);
	}

	void PTrack::AppendACFile(int segmentID)
	{
		std::fstream infile(GetTempACPathAndName()); // Get file stream from temporary AC file
		std::ofstream outfile;
		outfile.open(GetACPathAndName(), std::ios::app); // Get output stream for main AC file

		std::string line; // stores the current line

		// Ignore the fist 7 lines, not needed to describe a single segment
		for (int i = 0; i < 7; i++)
			std::getline(infile, line);

		// Read the rest of the file, edit as neccesary, and output to main AC file
		while (std::getline(infile, line))
		{
			std::size_t found = line.find("name");
			if (found != std::string::npos) // If the line contains a name, edit to correspond to segment ID
				line.at(line.length() - 2) = char(segmentID + '0');
			outfile << line + "\n"; // Output line to main file
		}

		infile.close();
		outfile.close();
	}

	const char *const PTrack::StrCon(const char *const a, const char *const b)
	{
		// Concatenates two char* and returns the result
		char* c = new char[strlen(a) + strlen(b)];
		c = strcpy(c, a);
		c = strcat(c, b);
		return c;
	}

	trackSeg* PTrack::GetStart() const
	{
		return start;
	}

	trackSeg* PTrack::GetEnd() const
	{
		return end;
	}

	void PTrack::SetStart(trackSeg* start)
	{
		this->start = start;
	}

	void PTrack::SetEnd(trackSeg* end)
	{
		this->end = end;
	}

	PTrack::~PTrack()
	{

	}

	/* Gets a segment from the cache */
	tTrackSeg* PTrack::GetSeg(int id)
	{
		tTrackSeg* curSeg = trk->seg;
		do {
			curSeg = curSeg->prev;
		} while (curSeg->id != id || curSeg == trk->seg);

		const char* segID = std::to_string(id).c_str();

		if (curSeg->id != id)
		{
			GfOut("Can't find segment with ID ");
			GfOut(segID);
			GfOut("!\n");
			return nullptr;
		}
		else return curSeg;
	}

	tdble PTrack::TotalLength() const
	{
		return totalLength;
	}

	tTrack* PTrack::BuildTrack(const char *const fName)
	{
		tTrack* track = (tTrack*)calloc(1, sizeof(tTrack));
		void* trackHandle;
		tRoadCam* theCamList = (tRoadCam*)NULL;

		track->params = trackHandle = GfParmReadFile(fName, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT | GFPARM_RMODE_PRIVATE);

		track->filename = _strdup(fName);

		GetTrackHeader(trackHandle, track);

		ReadTrack3(track, trackHandle, &theCamList, 0);

		// Initialize Sides
		InitSides(track->params, track);

		return track;
	}

#ifdef _DEBUG
	void PTrack::PrintSegsInOrder()
	{
		tTrackSeg* curSeg = start;

		do
		{
			std::cout << curSeg->name << std::endl;
			curSeg = curSeg->next;
		} while (curSeg != start);
	}

	void PTrack::PrintSegsBackwards()
	{
		tTrackSeg* curSeg = end;

		do
		{
			std::cout << curSeg->name << std::endl;
			curSeg = curSeg->prev;
		} while (curSeg != end);
	}
#endif // _DEBUG
}