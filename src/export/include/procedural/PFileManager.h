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
#include "trackdesc.h"

namespace procedural
{
#ifdef WIN32 // This class is only applicable to a windows build
	#define FILE_SEARCH_UNFILTERED "*"

	/* A singleton class to represent the file manager system used by the procedural library. WORKS WITH WINDOWS ONLY. */
	class PFileManager
	{
	private:
		// Private constructor - singleton object
		PFileManager();

		// Private copy and copy assignment
		PFileManager(const PFileManager& param) { };
		PFileManager& operator=(const PFileManager& param) { }

		static PFileManager* instance; /* Singleton instance of the file manager */
		
		char* CurrentDir; /* The executable directory, found upon instantiation of the singleton. */

		PTrackFileManager trkFileManager; /* A track file manager. Used to read and write tracks. Also stores track load state. */

		/*
		 * Copies an AC file from one file to another, creates the destination file if neccesary, and overwrites if it already exits.
		 * sourceFile The path and name of the source file.
		 * destFile	  The path and name of the destination file.
		 */
		void CopyACFile(std::string sourceFile, std::string destFile);

	public:
		virtual ~PFileManager();

		/* Get the singleton instance of the file manager class */
		static PFileManager* Get();

		/*
		 * Outputs the given track to a text file.
		 * trackName  The name of the track to output. Used to construct file names.
		 * configPath The path in which the track's configuration files can be found.
		 * configName The name of the configuration used by the track.
		 * track	  The TORCS track structure to output.
		 * category   The type of race that the track was generated within.
		*/
		void OutputTrack(std::string trackName, std::string configPath, std::string configName, tTrack* track);

		/* 
		 * Reads track segments from a file. Returns a vector containing all segments read.
		 * filePath The file to read segments from.
		 */
		std::vector<PSeg> ReadTrackSegments(std::string filePath);

		/* 
		 * Find the files with the given extension in the given directory.
		 * dirPath The path of the directory to search.
		 * fType   The file extension to search for. Leave blank to search for all files in the directory.
		 */
		std::vector<std::string> FilesInDirectory(std::string dirPath, std::string fType = FILE_SEARCH_UNFILTERED);

		/*
		 * Searches the given directory for a list of all directories it contains.
		 * dir The path of the directory to search for other directories.
		 */
		std::vector<std::string> DirectoriesInDirectory(std::string dir);

		/* 
		 * Set the track load state.
		 * loadState The load state to use.
		*/
		void SetTrackLoadState(const PTrackLoadState& loadState);

		/*
		 * Sets the length of the current track load state.
		 * length The length of the track that will be procedurally generated using the current load state.
		 */
		void SetTrackLoadStateLength(int length);

		/* Get the current track load state. */
		PTrackLoadState GetTrackLoadState();

		/* Returns the path that the current executable resides within */
		char* GetCurrentDir();
	};
#endif // WIN32
}
#endif // _P_FILE_MANAGER_H_