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

	void PFileManager::ConstructSegmentInput(const std::string& input)
	{
		// First, divide the input into lines
		std::vector<std::string> lines = std::vector<std::string>();
		lines.push_back("");
		int curLine = 0;

		for (int i = 0; i < input.length(); i++)
		{
			if (input[i] == '\n')
			{
				lines.push_back("");
				curLine++;
			}
			else
				lines[curLine].append((const char*)input[i]);
		}

		// Then, get the values from the lines
		int id = strToInt(lines[ID_LINE_IDX]);
		int type = strToInt(lines[TYPE_LINE_IDX]);
		tdble radius = strToInt(lines[RADIUS_LINE_IDX]);
		tdble length = strToInt(lines[LENGTH_LINE_IDX]);
		tdble arc = strToInt(lines[ARC_LINE_IDX]);

		// Finally, create the segment with the segment factory and add it the track
		switch (type)
		{
		case TR_STR:
			trackManager->AddSegment(PSegFactory::GetInstance()->CreateSegStr(id, length));
			break;
		case TR_LFT:
			trackManager->AddSegment(PSegFactory::GetInstance()->CreateSegCnr(id, PCornerType::CTLeft, radius, arc));
			break;
		case TR_RGT:
			trackManager->AddSegment(PSegFactory::GetInstance()->CreateSegCnr(id, PCornerType::CTRight, radius, arc));
			break;
		default:
			pOut("Error reading track segment, segmenti is of unknown type or type has been read incorrectly!\n");
			break;
		}
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

	void PFileManager::OutputTrack(std::string trackName, std::string fileName, std::string configPath, std::string configName, PTrackManager* trkMngr)
	{
		// Construct the filepath
		std::string filePath = configPath + "previousTracks/" + trackName + "/";
		
		// Construct a relative path for use with Gf functions
		std::string relativeCfgPath = "tracks/procedural/" + configName + "/";

		if (GfCreateDir((char*)filePath.c_str()))
		{
			filePath.append(fileName);

			// Open/Create file handle
			void* newHandle = GfParmReadFile(filePath.c_str(), GFPARM_RMODE_CREAT);

			// Get file handle of track configuration
			void* configHandle = GfParmReadFile((relativeCfgPath + configName + ".xml").c_str(), GFPARM_RMODE_STD);

			// Write the track and configuration into the handle
			trkFileManager.WriteTrackTo(newHandle, configHandle, trkMngr->GetTrack()->trk, trackName);

			// Set headers correctly
			GfParmSetDTD(newHandle, nullptr, "[<!ENTITY default - surfaces SYSTEM \"../../../../data/tracks/surfaces.xml\">]>");

			// Write file
			GfParmWriteFile(filePath.c_str(), newHandle, fileName.c_str());
		}
		else
			pOut("Error outputting track, could not create track directory!\n");
	}

	void PFileManager::ReadTrack(std::string fileName, PTrackManager* trkMngr)
	{
		// Obtain pointers
		trackManager = trkMngr;
		procTrack = trackManager->GetTrack();

		// Create stream and open file
		std::fstream stream;
		stream.open(fileName, std::ios::in);

		if (stream.is_open())
		{
			// Initialize segments vector
			std::vector<std::string> segments = std::vector<std::string>();
			segments.push_back(""); // Add an initial segment
			int lCount = 0;
			int segNumb = 0;

			char* c = new char;

			// Read each segment into an individual string
			while (stream.read(c, 1))
			{
				if (*c == '\n')
					lCount++;
				if (lCount > PARAM_COUNT)
				{
					segNumb++;
					segments.push_back("");
				}
				else
					segments[segNumb].append(c);
			}

			// Construct all segments
			for (auto i : segments)
				ConstructSegmentInput(i);
		}
		else
			pOut("Error, could not load file for track reading!\n");
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