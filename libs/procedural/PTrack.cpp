/*
	File: PTrack.cpp
	Author: Henri Keeble
	Desc: Defines a class used to represent a procedural track in TORCS. Encapsulates all data required to manage and update a procedural track.
*/
#include "PTrack.h"
#include "PTrackState.h"
#include "pDefs.h"

namespace procedural
{
	// Track Info Definitions
	PTrack::PTrack(tTrack* track, tdble totalLength, char* acname, char* xmlname,  char* filepath, ssgLoaderOptions* loaderoptions)
	{
		// Initialize TORCS Track Structure and Segment Collection
		this->trackCache = track;
		this->segs = PSegCollection();

		// Assign file path
		filePath = new char[strlen(filepath)];
		strcpy(filePath, filepath);

		// Assign paths and loader options for AC file handling
		acName = new char[strlen(filepath)];
		strcpy(acName, acname);

		this->loaderOptions = new ssgLoaderOptions(*loaderoptions);

		// Create temporary AC path and name
		tempACName = new char[strlen(acname) + 3];
		strcpy(tempACName, acname);
		strcat(tempACName, "tmp");

		// Create XML path and name
		this->xmlFile = new char[strlen(xmlname)];
		strcpy(xmlFile, xmlname);

		trackDesc = NULL;
		root = NULL;

		// Initialize track state
		state = PTrackState();

		this->totalLength = totalLength;
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
			if (loaderOptions)
				delete loaderOptions;
			if (trackDesc)
				delete trackDesc;

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

		if (param.loaderOptions)
		{
			loaderOptions = new ssgLoaderOptions();
			*loaderOptions = *param.loaderOptions;
		}
		else
			loaderOptions = nullptr;

		if (param.trackDesc)
			trackDesc = param.trackDesc;
		else
			trackDesc = nullptr;

		if (param.root)
			root = param.root;
		else
			root = nullptr;

		if (param.trackCache)
			trackCache = param.trackCache;
		else
			trackCache = nullptr;

		if (param.start)
			start = param.start;
		else
			start = nullptr;

		if (param.end)
			end = param.end;
		end = nullptr;
	}

	EntityDesc* PTrack::GetTrackDesc() const
	{
		return trackDesc;
	}

	void PTrack::SetTrackDesc(EntityDesc* newDesc)
	{
		trackDesc = newDesc;
	}

	const ssgLoaderOptions *const PTrack::GetLoaderOptions()
	{
		return loaderOptions;
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

	const char *const PTrack::StrCon(const char *const a, const char *const b)
	{
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
		if(trackDesc)
			delete trackDesc;
		if(acName)
			delete acName;
		if (filePath)
			delete filePath;
		if (xmlFile)
			delete xmlFile;
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