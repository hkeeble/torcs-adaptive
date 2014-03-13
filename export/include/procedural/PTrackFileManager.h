/*
	File: PTrackFileManager.h
	Author: Henri Keeble
	Desc: Declarations for a class designed to read and write track XML files for the procedural library.
*/

#ifndef _P_TRACK_FILE_MANAGER_
#define _P_TRACK_FILE_MANAGER_

#include <fstream>
#include <vector>

#include "PTrackConfig.h"

namespace procedural
{
	#define P_TRK_SECT_SEGS "Segments"
	#define P_TRK_SECT_HDR "Header"

	#define P_TRK_ATT_SEG_COUNT "Number of Segments"
	#define P_TRK_ATT_DESC "Description"
	#define P_TRK_ATT_NAME "Track Name"

	/* A class that represents a file manager designed to write out track files for the procedural library. */
	class PTrackFileManager
	{
	public:
		PTrackFileManager() { }
		~PTrackFileManager() { }
		
		/* Writes a track to the given track file */
		void WriteTrackTo(std::string filePath, std::string trkName, tTrack* track);

		/* Reads track data from the given file, and constructs a vector of procedural segment objects */
		std::vector<PSeg> ReadTrackFrom(std::string filePath);

	private:
		/* Writes the given track's segment data to the given track handle. */
		void WriteSegmentDataTo(void* newTrackHandle, tTrack* track);

		/* Writes an individual segment into a given handle */
		void WriteSegmentTo(void* newTrackHandle, tTrackSeg* seg);

		/* Reads segments from a given track handle */
		std::vector<PSeg> ReadSegmentDataFrom(void* trackHandle);

		/* Reads a single segment with the given name */
		PSeg ReadSegment(void* trackHandle, int id);
	};
}

#endif // _P_TRACK_FILE_MANAGER_