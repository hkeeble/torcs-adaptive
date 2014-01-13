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
	taTrack::taTrack(tTrack* track, tTrack* trackCache, char* acname, char* acpath, ssgLoaderOptions* loaderoptions)
	{
		this->track = track;
		this->trackCache = trackCache;
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
		tTrackSeg* lastSeg = end;
		tTrackSeg* firstSeg = start;

		firstSeg->next->prev = lastSeg;
		lastSeg->next = firstSeg->next;

		delete firstSeg;

		track->nseg -= 1;
	}

	void taTrack::RemoveSegAtEnd()
	{

	}

	void taTrack::AddSegmentAtStart()
	{

	}

	void taTrack::AddSegmentAtEnd()
	{

	}
}