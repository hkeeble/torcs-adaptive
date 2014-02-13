/*
	File: PTrack.cpp
	Author: Henri Keeble
	Desc: Defines a class used to represent a procedural track in TORCS. Encapsulates all data required to manage and update a procedural track.
*/
#include "PTrack.h"
#include "PTrackState.h"
#include "pDefs.h"
#include <fstream>

namespace procedural
{
	// Track Info Definitions
	PTrack::PTrack(tTrack* track, tdble totalLength, char* acname, char* xmlname,  char* filepath, ssgLoaderOptions* loaderoptions)
	{
		// Initialize TORCS Track Structure and Segment Collection
		this->trk = track;

		// Initialize track state
		state = PTrackState();
		root = nullptr;
		this->totalLength = totalLength;

		// Assign file path
		filePath = new char[strlen(filepath)];
		strcpy(filePath, filepath);

		// Assign paths and loader options for AC file handling
		acName = new char[strlen(filepath)];
		strcpy(acName, acname);

		// Create temporary AC path and name
		tempACName = new char[strlen(acname) + 3];
		strcpy(tempACName, acname);
		tempACName[strlen(acname) - 3] = 't';
		tempACName[strlen(acName) - 2] = 'm';
		tempACName[strlen(acName) - 1] = 'p';
		strcat(tempACName, ".ac");

		// Create XML path and name
		this->xmlFile = new char[strlen(xmlname)];
		strcpy(xmlFile, xmlname);

		// Initialize loader state
		ssgState = new PSSGState();
		ssgState->SetLoaderOptions(new ssgLoaderOptions(*loaderoptions));
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
			if (acName)
				delete acName;
			if (filePath)
				delete filePath;
			if (xmlFile)
				delete xmlFile;

			cpy(param);

			return *this;
		}
	}

	void PTrack::cpy(const PTrack& param)
	{
		state = param.state;

		if (param.filePath)
		{
			filePath = new char[strlen(param.filePath)];
			strcpy(filePath, param.filePath);
		}
		else
			filePath = nullptr;

		if (param.acName)
		{
			acName = new char[strlen(param.acName)];
			strcpy(acName, param.acName);
		}
		else
			acName = nullptr;

		if (param.xmlFile)
		{
			xmlFile = new char[strlen(param.xmlFile)];
			strcpy(xmlFile, param.xmlFile);
		}
		else
			xmlFile = nullptr;

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

	PSSGState* PTrack::GetSSGState()
	{
		return ssgState;
	}

	EntityDesc* PTrack::GetTrackDesc() const
	{
		return ssgState->GetDesc();
	}

	const ssgLoaderOptions *const PTrack::GetLoaderOptions()
	{
		return ssgState->GetLoaderOptions();
	}

	const char *const PTrack::GetACName()
	{
		return acName;
	}

	const char *const PTrack::GetACPathAndName()
	{
		return StrCon(filePath, acName);
	}

	const char *const PTrack::GetTempACName()
	{
		return tempACName;
	}


	const char *const PTrack::GetTempACPathAndName()
	{
		return StrCon(filePath, tempACName);
	}

	const char *const PTrack::GetXMLName()
	{
		return xmlFile;
	}

	const char *const PTrack::GetXMLPathAndName()
	{
		return StrCon(filePath, xmlFile);
	}

	const char *const PTrack::GetFilePath()
	{
		return filePath;
	}

	void PTrack::UpdateACFile(int segmentID)
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
		if(acName)
			delete acName;
		//if (filePath)
			//delete filePath;
		//if (xmlFile)
			//delete xmlFile;
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