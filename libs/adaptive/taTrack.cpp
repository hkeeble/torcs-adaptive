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
			tTrackSeg* lastSeg = end;
			tTrackSeg* firstSeg = start;

			firstSeg->next->prev = lastSeg;
			lastSeg->next = firstSeg->next;
			start = firstSeg->next;

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
			tTrackSeg* lastSeg = end;
			tTrackSeg* firstSeg = start;

			lastSeg->prev->next = firstSeg;
			firstSeg->prev = lastSeg->prev;
			end = lastSeg->prev;
			track->seg = end;

			delete lastSeg;

			track->nseg -= 1;
		}
		else
			GfOut("Attempted to remove segment, but only one exists!\n");
	}

	void taTrack::AddSegmentAtStart()
	{
		if (track->nseg < trackCache.nSegs())
		{
			// Pointers used for clarity
			tTrackSeg* lastSeg = end;
			tTrackSeg* firstSeg = start;

			// Find the segment to be added in the cache
			tTrackSeg* segToAdd = trackCache(firstSeg->id - 1);
			
			if (segToAdd == nullptr)
			{
				GfOut("Error finding segment to add in cache!\n");
				return;
			}

			// Allocate memory for the segment in the track
			firstSeg->prev = new tTrackSeg();

			*firstSeg->prev = *segToAdd; // Copy segment from cache to track

			// Reassign first seg and start
			firstSeg = firstSeg->prev;
			start = firstSeg;

			// Link last segment to new segment
			firstSeg->next = lastSeg;
			lastSeg->prev = firstSeg;
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
			tTrackSeg* segToAdd = trackCache(lastSeg->id + 1);

			if (segToAdd == nullptr)
			{
				GfOut("Error finding segment to add in cache!\n");
				return;
			}

			lastSeg->next = new tTrackSeg(); // Allocate memory for seg

			*lastSeg->next = *segToAdd; // Copy seg from cache

			// Reassign end
			lastSeg = lastSeg->next;
			end = lastSeg;

			// Link first segment to new segment
			firstSeg->prev = lastSeg;
			lastSeg->next = firstSeg;
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
}