/*
	File: PFileManager.h
	Author: Henri Keeble
	Desc: Declares a class designed to handle files representing procedural tracks.
*/

#ifndef _P_FILE_MANAGER_H_
#define _P_FILE_MANAGER_H_

#include <string>
#include <sstream>
#include <fstream>

#include "pSeg.h"
#include "PTrackManager.h"
#include "PTrackFileManager.h"
#include "trackgen\trackgen.h"
#include "track.h"

namespace procedural
{
#ifdef WIN32 // This class is only applicable to a windows build
	#define FILE_SEARCH_UNFILTERED "*"

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

		PTrackFileManager trkFileManager;

	public:
		/* Get the singleton instance of the file manager class */
		static PFileManager* Get();

		/* Outputs the current track to a text file */
		void OutputTrack(std::string trackName, std::string configPath, std::string configName, tTrack* track);

		/* Reads track segments from a file */
		std::vector<PSeg> ReadTrackSegments(std::string filePath);

		/* Find the files with the given extension in the given directory, leave extension blank for all files */
		std::vector<std::string> FilesInDirectory(std::string dirPath, std::string fType = FILE_SEARCH_UNFILTERED);

		/* Searches the given directory for a list of all directories it contains */
		std::vector<std::string> DirectoriesInDirectory(std::string dir);

		/* Returns the path that the current executable resides within */
		char* GetCurrentDir();
	};
#endif // WIN32
}
#endif // _P_FILE_MANAGER_H_