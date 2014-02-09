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
		this->trackCache = track;
		this->segs = PSegCollection();

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

		if (param.trackCache)
			trackCache = new tTrack(*param.trackCache);
		else
			trackCache = nullptr;

		if (param.start)
		{
			tTrackSeg* curSeg = trackCache->seg;
			while (curSeg->id != param.start->id)
				curSeg = curSeg->prev;
			start = curSeg;
		}
		else
			start = nullptr;

		if (param.end)
		{
			tTrackSeg* curSeg = trackCache->seg;
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

		segs = param.segs;
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

	void PTrack::RemoveSegAtStart()
	{
		if (trackCache->nseg > 1)
		{
			// Pointers obtained for code clarity
			tTrackSeg* lastSeg = end;
			tTrackSeg* firstSeg = start;

			// Adjust length
			trackCache->length -= start->length;

			// Re-arrange pointers accordingly
			firstSeg->next->prev = lastSeg;
			lastSeg->next = firstSeg->next;
			start = firstSeg->next;

			// Delete first segment from memory - is stored in trackCache for re-enstatement
			delete firstSeg;

			trackCache->nseg -= 1;
		}
		else
			GfOut("Attempted to remove segment, but only one exists!\n");
	}

	void PTrack::RemoveSegAtEnd()
	{
		if (trackCache->nseg > 1)
		{
			// Pointers obtained for code clarity
			tTrackSeg* lastSeg = end;
			tTrackSeg* firstSeg = start;

			// Adjust track length
			trackCache->length -= end->length;

			// Rearrange pointers accordingly
			lastSeg->prev->next = firstSeg;
			firstSeg->prev = lastSeg->prev;
			end = lastSeg->prev;
			trackCache->seg = end;

			// Delete last segment from memory - is stored in trackCache for re-enstatement
			delete lastSeg;

			trackCache->nseg -= 1;
		}
		else
			GfOut("Attempted to remove segment, but only one exists!\n");
	}

	void PTrack::AddSegmentAtStart()
	{
		GfOut("Adding TORCS-Adaptive Segment from cache...\n");

		if (trackCache->nseg < segs.nSegs())
		{
			// Pointers used for clarity
			tTrackSeg* lastSeg = end;
			tTrackSeg* firstSeg = start;

			// Find the segment to be added in the cache
			if (firstSeg->id - 1 < 0)
			{
				GfOut("Error: Cannot add segment with negative ID!\n");
				return;
			}

			tTrackSeg* segToAdd = new tTrackSeg();
			*segToAdd = *segs(firstSeg->id - 1);

			// Rearrange Pointers
			segToAdd->prev = lastSeg;
			segToAdd->next = firstSeg;
			firstSeg->prev = segToAdd;
			lastSeg->next = segToAdd;
			start = segToAdd;

			// Adjust Track Length
			trackCache->length += segToAdd->length;

			trackCache->nseg++;
		}
		else
			GfOut("Attempted to add segment, but no more stored in the cache!\n");
	}

	void PTrack::AddSegmentAtEnd()
	{
		if (trackCache->nseg < segs.nSegs())
		{
			// Pointers used for clarity
			tTrackSeg* lastSeg = end;
			tTrackSeg* firstSeg = start;

			// Find the segment to be added in the cache
			if (firstSeg->id + 1 > segs.nSegs())
			{
				GfOut("Error: Cannot add segment to cache, does not exist!\n");
				return;
			}

			tTrackSeg* segToAdd = new tTrackSeg();
			*segToAdd = *segs(lastSeg->id + 1);

			// Rearrange Pointers
			segToAdd->prev = lastSeg;
			segToAdd->next = firstSeg;
			lastSeg->next = segToAdd;
			firstSeg->prev = segToAdd;
			end = segToAdd;

			// Adjust Track Length
			trackCache->length += segToAdd->length;

			trackCache->nseg++;
		}
		else
			GfOut("Attempted to add segment, but no more stored in the cache!\n");
	}

	/* Gets a segment from the cache */
	tTrackSeg* PTrack::GetSeg(int id)
	{
		tTrackSeg* curSeg = trackCache->seg;
		do {
			curSeg = curSeg->prev;
		} while (curSeg->id != id || curSeg == trackCache->seg);

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

	tTrack* PTrack::BuildTrack()
	{
		taOut("Building entire track for output...\n");
		for (int i = 0; i < segs.nSegs(); i++)
			AddSegmentAtStart();
		taOut("Track built!\n");

		return trackCache;
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