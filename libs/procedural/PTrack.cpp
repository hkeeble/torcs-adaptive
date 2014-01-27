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
	PTrack::PTrack(tTrack* track, char* acname, char* acpath, ssgLoaderOptions* loaderoptions)
	{
		// Initialize TORCS Track Structure and Segment Collection
		this->trackCache = track;
		this->segs = PSegCollection();

		// Assign paths and loaer options for AC file handling
		acName = acname;
		acPath = acpath;
		loaderOptions = loaderoptions;

		trackDesc = NULL;
		root = NULL;

		// Initialize track state
		state = PTrackState();
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
			if (acPath)
				delete acPath;
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

		if (param.acPath)
		{
			acPath = new char[strlen(param.acPath)];
			strcpy(acPath, param.acPath);
		}
		else
			acPath = nullptr;

		if (param.acName)
		{
			acName = new char[strlen(param.acName)];
			strcpy(acName, param.acName);
		}
		else
			acName = nullptr;

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

	const char *const PTrack::GetACPath()
	{
		return acPath;
	}

	const char *const PTrack::GetACPathAndName()
	{
		char* c = new char[strlen(acName) + strlen(acPath)];
		c = strcpy(c, acPath);
		c = strcat(c, acName);
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
		if(acPath)
			delete acPath;
	}

	void PTrack::RemoveSegAtStart()
	{
		if (trackCache->nseg > 1)
		{
			// Pointers obtained for code clarity
			tTrackSeg* lastSeg = end;
			tTrackSeg* firstSeg = start;

			// Adjust track length
			if (start->type == TR_STR)
				trackCache->length -= start->length;
			else
				trackCache->length -= start->arc;

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
			if (end->type == TR_STR)
				trackCache->length -= end->length;
			else
				trackCache->length -= end->arc;

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
			if (segToAdd->type == TR_STR)
				trackCache->length += segToAdd->length;
			else
				trackCache->length += segToAdd->arc;

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
			if (segToAdd->type == TR_STR)
				trackCache->length += segToAdd->length;
			else
				trackCache->length += segToAdd->arc;

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