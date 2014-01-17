/*
	File: taTrackInfo.cpp
	Desc: definitions file for torcsAdaptive track info class
	Author: Henri Keeble
*/
#include "torcsAdaptive\taTrack.h"
#include "torcsAdaptive\taTrackState.h"
#include "torcsAdaptive\taDefs.h"

namespace torcsAdaptive
{
	// Track Info Definitions
	taTrack::taTrack(tTrack* track, char* acname, char* acpath, ssgLoaderOptions* loaderoptions)
	{
		this->track = track;
		this->trackCache = taTrackCache();
		acName = acname;
		acPath = acpath;
		loaderOptions = loaderoptions;
		trackDesc = NULL;
		root = NULL;
		state = taTrackState();
	}

	taTrack::taTrack(const taTrack& param)
	{
		cpy(param);
	}

	taTrack& taTrack::operator=(const taTrack& param)
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

	void taTrack::cpy(const taTrack& param)
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

		if (param.track)
			track = param.track;
		else
			track = nullptr;

		if (param.start)
			start = param.start;
		else
			start = nullptr;

		if (param.end)
			end = param.end;
		end = nullptr;
	}

	EntityDesc* taTrack::GetTrackDesc() const
	{
		return trackDesc;
	}

	void taTrack::SetTrackDesc(EntityDesc* newDesc)
	{
		trackDesc = newDesc;
	}

	const ssgLoaderOptions *const taTrack::GetLoaderOptions()
	{
		return loaderOptions;
	}

	const char *const taTrack::GetACName()
	{
		return acName;
	}

	const char *const taTrack::GetACPath()
	{
		return acPath;
	}

	const char *const taTrack::GetACPathAndName()
	{
		char* c = new char[strlen(acName) + strlen(acPath)];
		c = strcpy(c, acPath);
		c = strcat(c, acName);
		return c;
	}

	trackSeg* taTrack::GetStart() const
	{
		return start;
	}

	trackSeg* taTrack::GetEnd() const
	{
		return end;
	}

	void taTrack::SetStart(trackSeg* start)
	{
		this->start = start;
	}

	void taTrack::SetEnd(trackSeg* end)
	{
		this->end = end;
	}

	taTrack::~taTrack()
	{
		if(trackDesc)
			delete trackDesc;
		if(acName)
			delete acName;
		if(acPath)
			delete acPath;
	}

	void taTrack::RemoveSegAtStart()
	{
		if (track->nseg > 1)
		{
			// Pointers obtained for code clarity
			tTrackSeg* lastSeg = end;
			tTrackSeg* firstSeg = start;

			// Adjust track length
			if (start->type == TR_STR)
				track->length -= start->length;
			else
				track->length -= start->arc;

			// Re-arrange pointers accordingly
			firstSeg->next->prev = lastSeg;
			lastSeg->next = firstSeg->next;
			start = firstSeg->next;

			// Delete first segment from memory - is stored in trackCache for re-enstatement
			delete firstSeg;

			track->nseg -= 1;
		}
		else
			GfOut("Attempted to remove segment, but only one exists!\n");
	}

	void taTrack::RemoveSegAtEnd()
	{
		if (track->nseg > 1)
		{
			// Pointers obtained for code clarity
			tTrackSeg* lastSeg = end;
			tTrackSeg* firstSeg = start;

			// Adjust track length
			if (end->type == TR_STR)
				track->length -= end->length;
			else
				track->length -= end->arc;

			// Rearrange pointers accordingly
			lastSeg->prev->next = firstSeg;
			firstSeg->prev = lastSeg->prev;
			end = lastSeg->prev;
			track->seg = end;

			// Delete last segment from memory - is stored in trackCache for re-enstatement
			delete lastSeg;

			track->nseg -= 1;
		}
		else
			GfOut("Attempted to remove segment, but only one exists!\n");
	}

	void taTrack::AddSegmentAtStart()
	{
		GfOut("Adding TORCS-Adaptive Segment from cache...\n");

		if (track->nseg < trackCache.nSegs())
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
			*segToAdd = *trackCache(firstSeg->id - 1);

			segToAdd->prev = lastSeg;
			segToAdd->next = firstSeg;
			firstSeg->prev = segToAdd;
			lastSeg->next = segToAdd;
			start = segToAdd;

			if (segToAdd->type == TR_STR)
				track->length += segToAdd->length;
			else
				track->length += segToAdd->arc;

			track->nseg++;
		}
		else
			GfOut("Attempted to add segment, but no more stored in the cache!\n");
	}

	void taTrack::AddSegmentAtEnd()
	{
		if (track->nseg < trackCache.nSegs())
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
			*segToAdd = *trackCache(lastSeg->id + 1);

			segToAdd->prev = lastSeg;
			segToAdd->next = firstSeg;
			lastSeg->next = segToAdd;
			firstSeg->prev = segToAdd;
			end = segToAdd;

			if (segToAdd->type == TR_STR)
				track->length += segToAdd->length;
			else
				track->length += segToAdd->arc;

			track->nseg++;
		}
		else
			GfOut("Attempted to add segment, but no more stored in the cache!\n");
	}

	tTrackSeg* taTrack::GetSeg(int id)
	{
		tTrackSeg* curSeg = track->seg;
		do {
			curSeg = curSeg->prev;
		} while (curSeg->id != id || curSeg == track->seg);

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
	void taTrack::PrintSegsInOrder()
	{
		tTrackSeg* curSeg = start;

		do
		{
			std::cout << curSeg->name << std::endl;
			curSeg = curSeg->next;
		} while (curSeg != start);
	}

	void taTrack::PrintSegsBackwards()
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