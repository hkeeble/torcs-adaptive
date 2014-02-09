/*
	File: PTrack.h
	Author: Henri Keeble
	Desc: A class used to represent a procedural track in TORCS. Encapsulates all data required to manage and update a procedural track.
*/

#ifndef _P_TRACK_H_
#define _P_TRACK_H_

#include <fstream>
#include "PTrackState.h"
#include "PDefs.h"
#include "PSegCollection.h"
#include "PCarData.h"
#include "PSSGState.h"
#include "track.h"
#include "../../../windows/include/plib/ssg.h"

// Forward declarations (inclusion of track would result in circular inclusion)
struct trackSeg;

namespace procedural
{
	/* Represents the lifetime of a procedural track */
	class PTrack
	{
	private:
		PSSGState* ssgState;
		char* filePath;
		char* xmlFile;
		char* acName;
		char* tempACName;
		trackSeg *start, *end;

		// Total intended track length
		tdble totalLength;

		/* Internal copying function */
		void cpy(const PTrack& param);

		/* Concatenates two char and returns the result. */
		const char *const StrCon(const char *const a, const char *const b);

	public:

		/* Number of segments stored in memory either side of occupied segment */
		const int SEG_MEMORY_SIZE = 3;

		PTrack(tTrack* track, tdble totalLength, char* acname, char* xmlname, char* filepath, ssgLoaderOptions* loaderoptions);
		~PTrack();
		PTrack(const PTrack& param);
		PTrack& operator=(const PTrack& param);

		/* Current track state */
		PTrackState state;
		
		/* Root of track (the last added segment) */
		trackSeg* root;
		
		/* The TORCS track structure, only contains a cache of segments */
		tTrack* trackCache;

		/* Collection for storing all segments */
		PSegCollection segs;

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

		/* Get the total INTENDED track length - not the current total length */
		tdble TotalLength() const;

		/* Builds the entire track into the existing cache. Returns the resulting track, but also modifies the internal track structure. */
		tTrack* BuildTrack();

		/* Appends the track's AC file with data stored in temporary AC file */
		void UpdateACFile(int segmentID);

		/* Get Accessors */
		const ssgLoaderOptions *const GetLoaderOptions();
		const char			   *const GetFilePath();
		const char			   *const GetACName();
		const char			   *const GetACPathAndName();
		const char			   *const GetTempACName();
		const char			   *const GetTempACPathAndName();
		const char			   *const GetXMLName();
		const char			   *const GetXMLPathAndName();
		PSSGState*					  GetSSGState();
		EntityDesc*					  GetTrackDesc() const;
		trackSeg*					  GetStart() const;
		trackSeg*					  GetEnd() const;

		/* Set Accessors */
		void SetStart(trackSeg* start);
		void SetEnd(trackSeg* end);

		/* Debugging Functions */
#ifdef _DEBUG
		void PrintSegsInOrder();
		void PrintSegsBackwards();
#endif // _DEBUG
	};
}

#endif _P_TRACK_H