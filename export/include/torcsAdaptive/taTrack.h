/*
	File: taTrackInfo.h
	Desc: header file for torcsAdaptive track info class
	Author: Henri Keeble
*/

#ifndef _TA_TRACK_INFO_H_
#define _TA_TRACK_INFO_H_

#include "taTrackState.h"
#include "taDefs.h"
#include "taTrackCache.h"
#include "track.h"
#include "../../../windows/include/plib/ssg.h"

// Forward declarations (inclusion of track would result in circular inclusion)
struct trackSeg;

namespace torcsAdaptive
{
	/* Represents the lifetime of an adaptive track */
	class taTrack
	{
	private:
		EntityDesc* trackDesc;
		ssgLoaderOptions* loaderOptions;
		char* acName;
		char* acPath;
		trackSeg *start, *end;

		/* Internal copying function */
		void cpy(const taTrack& param);

	public:

		/* Number of segments stored in memory either side of occupied segment */
		const int SEG_MEMORY_SIZE = 3;

		taTrack(tTrack* track, char* acname, char* acpath, ssgLoaderOptions* loaderoptions);
		~taTrack();
		taTrack(const taTrack& param);
		taTrack& operator=(const taTrack& param);

		/* Current track state */
		taTrackState state;
		
		/* Root of track (the last added segment) */
		trackSeg* root;
		
		/* The TORCS track structure */
		tTrack* track;

		/* Cache for storing all segments */
		taTrackCache trackCache;

		/* Remove a segment at the start */
		void RemoveSegAtStart();

		/* Remove a segment at the end */
		void RemoveSegAtEnd();

		/* Add a segment at the start */
		void AddSegmentAtStart();

		/* Add a segment at the end */
		void AddSegmentAtEnd();

		/* Get segment with the specified ID */
		tTrackSeg* GetSeg(int id);

		/* Get Accessors */
		const ssgLoaderOptions *const GetLoaderOptions();
		const char			   *const GetACName();
		const char			   *const GetACPath();
		const char			   *const GetACPathAndName();
		EntityDesc*					  GetTrackDesc() const;
		trackSeg*					  GetStart() const;
		trackSeg*					  GetEnd() const;

		// Set Accessors
		void SetTrackDesc(EntityDesc* newDesc);
		void SetStart(trackSeg* start);
		void SetEnd(trackSeg* end);
	};
}

#endif _TA_TRACK_INFO_H