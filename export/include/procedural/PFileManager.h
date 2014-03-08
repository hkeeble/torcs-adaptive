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
#ifdef WIN32 // This class is only applicable to a windows build

	// Track input and output defines
	#define TRACK_OUT_FTYPE ".trk" // Track output filetype

		// Line indices for reading
	#define PARAM_COUNT		5
	#define ID_LINE_IDX		0
	#define TYPE_LINE_IDX	1
	#define LENGTH_LINE_IDX 2
	#define RADIUS_LINE_IDX 3
	#define ARC_LINE_IDX	4

	/* A singleton class to represent the file manager system used by the procedural library. WORKS WITH WINDOWS ONLY. */
	class PFileManager
	{
	private:
		PFileManager();

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

		// The current directory. This is found upon instantiation of the singleton
		char* CurrentDir;

	public:
		/* Get the singleton instance of the file manager class */
		static PFileManager* Get();

		/* Outputs the current track to a text file */
		void OutputTrack(std::string fileName, PTrackManager* trkMngr);

		/* Reads a track from a text file */
		void ReadTrack(std::string fileName, PTrackManager* trkMngr);

		/* Find the files with the given extension in the given directory, leave extension blank for all files */
		std::vector<std::string> FilesInDirectory(std::string dirPath, std::string fType = "*");

		/* Searches the given directory for a list of all directories it contains */
		std::vector<std::string> DirectoriesInDirectory(std::string dir);

		/* Returns the path that the current executable resides within */
		char* GetCurrentDir();
	};
#endif // WIN32
}
#endif // _P_FILE_MANAGER_H_