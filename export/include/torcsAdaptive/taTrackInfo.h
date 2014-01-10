/*
	File: taTrackInfo.h
	Desc: header file for torcsAdaptive track info class
	Author: Henri Keeble
*/

#ifndef _TA_TRACK_INFO_H_
#define _TA_TRACK_INFO_H_

#include "taTrackState.h"
#include "taDefs.h"
#include "../../../windows/include/plib/ssg.h"

struct trackSeg;

namespace torcsAdaptive
{
	/* Represents the lifetime of an adaptive track */
	class taTrackInfo
	{
	private:
		EntityDesc* trackDesc;
		ssgLoaderOptions* loaderOptions;
		char* acName;
		char* acPath;
		trackSeg *start, *end;

	public:
		taTrackInfo(char* acname, char* acpath, ssgLoaderOptions* loaderoptions);
		~taTrackInfo();
		taTrackInfo(const taTrackInfo& param);
		taTrackInfo& operator=(const taTrackInfo& param);

		taTrackState state;
		trackSeg* root;
		
		// Get Accessors
		const ssgLoaderOptions *const GetLoaderOptions();
		const char			   *const GetACName();
		const char			   *const GetACPath();
		const char			   *const GetACPathAndName();
		EntityDesc*	GetTrackDesc() const;
		trackSeg* GetStart() const;
		trackSeg* GetEnd() const;

		// Set Accessors
		void SetTrackDesc(EntityDesc* newDesc);
		void SetStart(trackSeg* start);
		void SetEnd(trackSeg* end);
	};
}

#endif _TA_TRACK_INFO_H