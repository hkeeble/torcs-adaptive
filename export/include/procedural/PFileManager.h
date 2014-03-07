/*
	File: PFileManager.h
	Author: Henri Keeble
	Desc: Declares a class designed to handle files representing procedural tracks.
*/

#ifndef _P_FILE_MANAGER_H_
#define _P_FILE_MANAGER_H_

#include <string>
#include <sstream>

#include "pSeg.h"
#include "PTrackManager.h"
#include "track.h"

namespace procedural
{
	// Track input and output defines
	#define TRACK_OUT_FTYPE ".trk" // Track output filetype

		// Line indices for reading
	#define PARAM_COUNT		5
	#define ID_LINE_IDX		0
	#define TYPE_LINE_IDX	1
	#define LENGTH_LINE_IDX 2
	#define RADIUS_LINE_IDX 3
	#define ARC_LINE_IDX	4

	/* A singleton class to represent the file manager system used by the procedural library */
	class PFileManager
	{
	private:
		PFileManager() : trackManager(nullptr) { };
		
		static PFileManager* instance;
		
		/* Construct a string for a single segment ready for output. */
		std::string ConstructSegmentOutput(tTrackSeg* seg);

		/* Constructs a single segment from the given input string, and adds it to the track. */
		void ConstructSegmentInput(const std::string& input);
	
		/* Converts a string to an integer */
		int strToInt(std::string string);
	
		/* Pointer to the current track manager and other track objects */
		PTrackManager* trackManager;
		PTrack* procTrack;

	public:
		/* Get the singleton instance of the file manager class */
		static PFileManager* Get();
		
		/* Outputs the current track to a text file */
		void OutputTrack(std::string fileName, PTrackManager* trkMngr);

		/* Reads a track from a text file */
		void ReadTrack(std::string fileName, PTrackManager* trkMngr);
	};
}

#endif // _P_FILE_MANAGER_H_