/*
	File: taTrackInfo.cpp
	Desc: definitions file for torcsAdaptive track info class
	Author: Henri Keeble
*/
#include "torcsAdaptive\taTrackInfo.h"
#include "torcsAdaptive\taTrackState.h"
#include "torcsAdaptive\taDefs.h"

namespace torcsAdaptive
{
	// Track Info Definitions
	taTrackInfo::taTrackInfo(char* acname, char* acpath, ssgLoaderOptions* loaderoptions)
	{
		acName = acname;
		acPath = acpath;
		loaderOptions = loaderoptions;
		trackDesc = NULL;
		root = NULL;
		state = taTrackState();
	}

	taTrackInfo::taTrackInfo(const taTrackInfo& param)
	{

	}

	taTrackInfo& taTrackInfo::operator=(const taTrackInfo& param)
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

			if (param.acPath)
			{
				acPath = new char[strlen(param.acPath)];
				strcpy(acPath, param.acPath);
			}
			else
				acPath = NULL;

			if (param.acName)
			{
				acName = new char[strlen(param.acName)];
				strcpy(acName, param.acName);
			}
			else
				acName = NULL;

			if (param.loaderOptions)
			{
				loaderOptions = new ssgLoaderOptions();
				*loaderOptions = *param.loaderOptions;
			}
			else
				loaderOptions = NULL;

			if (param.trackDesc)
				trackDesc = param.trackDesc;
			else
				trackDesc = NULL;

			return *this;
		}
	}

	EntityDesc* taTrackInfo::GetTrackDesc() const
	{
		return trackDesc;
	}

	void taTrackInfo::SetTrackDesc(EntityDesc* newDesc)
	{
		trackDesc = newDesc;
	}

	const ssgLoaderOptions *const taTrackInfo::GetLoaderOptions()
	{
		return loaderOptions;
	}

	const char *const taTrackInfo::GetACName()
	{
		return acName;
	}

	const char *const taTrackInfo::GetACPath()
	{
		return acPath;
	}

	const char *const taTrackInfo::GetACPathAndName()
	{
		char* c = new char[strlen(acName) + strlen(acPath)];
		c = strcpy(c, acPath);
		c = strcat(c, acName);
		return c;
	}

	trackSeg* taTrackInfo::GetStart() const
	{
		return start;
	}

	trackSeg* taTrackInfo::GetEnd() const
	{
		return end;
	}

	void taTrackInfo::SetStart(trackSeg* start)
	{
		this->start = start;
	}

	void taTrackInfo::SetEnd(trackSeg* end)
	{
		this->end = end;
	}

	taTrackInfo::~taTrackInfo()
	{
		if(trackDesc)
			delete trackDesc;
		if(acName)
			delete acName;
		if(acPath)
			delete acPath;
	}
}