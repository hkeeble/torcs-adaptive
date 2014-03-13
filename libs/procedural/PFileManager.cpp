/*
	File: PFileManager.cpp
	Author: Henri Keeble
	Desc: Defines the file management class used by the procedural library.
*/

#include "PFileManager.h"

namespace procedural
{
	PFileManager* PFileManager::instance = nullptr;

	PFileManager::PFileManager() : trackManager(nullptr)
	{
		/* Get the current directory (as this will not change) */

		// Get Path
		TCHAR path[MAX_PATH];
		GetModuleFileName(NULL, path, MAX_PATH);

		// Convert to string
		std::stringstream ss;
		ss << path;
		std::string s = ss.str();

		// Remove Executable Name
		unsigned int pos = s.find_last_of("\\");
		s = s.substr(0, pos + 1);

		char* realPath = (char*)calloc(strlen(s.c_str()), sizeof(char));
		strcpy(realPath, s.c_str());

		CurrentDir = realPath;

		// Initialize a track file manager
		trkFileManager = PTrackFileManager();
	};

	std::string PFileManager::ConstructSegmentOutput(tTrackSeg* seg)
	{
		std::string string = "";

		string += std::to_string(seg->id); // Ouput ID
		string += "\n";
		string += std::to_string(seg->type); // Output Type
		string += "\n";
		string += std::to_string(seg->length); // Output Length
		string += "\n";
		string += std::to_string(seg->radius); // Output Radius
		string += "\n";
		string += std::to_string(seg->arc); // Output arc
		string += "\n";

		return string;
	}

	

	int PFileManager::strToInt(std::string string)
	{
		std::stringstream ss = std::stringstream(string);
		int ret = 0;
		if (!(ss >> ret))
			pOut("Error, string to int conversion failed!\n");
		return ret;
	}

	PFileManager* PFileManager::Get()
	{
		if (instance == nullptr)
			instance = new PFileManager();
		return instance;
	}

	void PFileManager::OutputTrack(std::string trackName, std::string configPath, std::string configName, tTrack* track)
	{
		pOut("Outputting track file...\n");

		// Construct the filepath
		std::string filePath = configPath + "previousTracks/" + trackName + "/";
		
		// Construct a relative path for use with Gf functions
		std::string relativeCfgPath = "tracks/procedural/" + configName + "/";

		if (GfCreateDir((char*)filePath.c_str()))
		{
			// Append track name to file path
			filePath.append(trackName + ".xml");

			// Write the track and configuration into the handle
			trkFileManager.WriteTrackTo(filePath, trackName, track);

			pOut("Outputting track 3D Description...\n");

			// Write out 3D description of entire track
			GenerateTrack(track, track->params, const_cast<char*>(std::string(relativeCfgPath + "previousTracks/" + trackName + "/" + trackName + ".ac").c_str()), nullptr, 0, 0, 0);

			pOut("Track files output successfully!\n");
		}
		else
			pOut("Error outputting track, could not create track directory!\n");
	}

	std::vector<PSeg> PFileManager::ReadTrackSegments(std::string filePath)
	{
		return trkFileManager.ReadTrackFrom(filePath);
	}

	std::vector<std::string> PFileManager::FilesInDirectory(std::string dirPath, std::string fType)
	{
		// Declare variables
		std::vector<std::string> files = std::vector<std::string>();
		FList* filesList;
		FList* curFile;

		// Construct a directory list
		if (fType != FILE_SEARCH_UNFILTERED)
			filesList = GfDirGetListFiltered(dirPath.c_str(), fType.c_str());
		else
			filesList = GfDirGetList(dirPath.c_str());
		curFile = filesList;

		// Read the directory list into a vector
		do {
			if (std::string(curFile->name).find(fType, 0) != std::string::npos)
				files.push_back(curFile->name);
			curFile = curFile->next;
		} while (curFile != filesList);

		// Fre the directory list
		GfDirFreeList(filesList, nullptr, true, true);

		// Return list of files
		return files;
	}

	std::vector<std::string> PFileManager::DirectoriesInDirectory(std::string dir)
	{
		std::vector<std::string> files = std::vector<std::string>();

		HANDLE hFind = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATAA fi;

		hFind = FindFirstFile((dir + "*").c_str(), &fi);

		// Handle errors
		if (hFind == INVALID_HANDLE_VALUE)
		{
			pOut("Error searching directory for other directories: Failed to find any files!\n");
			return files;
		}
		
		// Build list of directories
		do {
			if (fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (fi.cFileName != "." && fi.cFileName != "..")
					files.push_back(fi.cFileName);
			}
		} while (FindNextFile(hFind, &fi));

		return files;
	}

	char* PFileManager::GetCurrentDir()
	{
		return CurrentDir;
	}
}