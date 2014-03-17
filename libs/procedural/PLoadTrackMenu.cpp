/*
	File: PLoadTrackMenu.cpp
	Author: Henri Keeble
	Desc: Contains functions to create a menu used to load tracks.
*/

#include "PMenu.h"
#include "tgfclient.h"
#include "raceinit.h"
#include <string>

namespace procedural
{
	// Screen handle
	static void* handle;

	// Scroll list id
	static int trkListID;
	static int folderListID;

	// Window width and height
	static const int wWidth = 800;
	static const int wHeight = 600;

	static std::string selectedTrack = "";
	static std::string selectedFolder = "";

	static std::string selectedTrackPath = "";
	static std::string selectedFolderPath = "";

	// Pointer to the file manager
	static PFileManager* fileManager;

	// Scroll List Contents
	static std::vector<std::string>* dirs;
	static std::vector<std::string>* trkDirs;

	// Paths - user data
	static std::vector<std::string>* dirPaths;
	static std::vector<std::string>* trkPaths;

	// Callback function prototypes
	static void PFolderSelectListClick(void* /* dummy param */);
	static void PTrackSelectListClick (void* /* dummy param */);
	static void PAcceptTrackSelectMenu(void* ts);
	static void PCancelTrackSelectMenu(void* ts);

	// Reset function - resets the screen
	static void PResetLoadScreen();

	// Utility functions
	static void PUpdateTrackList(void* /* dummy param */);

	// Some parameters to track
	static int nOfTracksInScrollList = 0;

	/* Creates and displays the menu. Retrieves a pointer to a PTrackSelectParams menu, as this contains pointers to the previous and next screens that must be visited */
	void PCreateTrackSelectMenu(void* params)
	{
		// Cast pointer to parameters
		PMenuParams* p = (PMenuParams*)params;

		// Obtain pointer to file manager
		fileManager = PFileManager::Get();

		// Create a screen handle
		handle = GfuiScreenCreate();

		// Initialize scroll list contents
		dirs = new std::vector<std::string>();
		trkDirs = new std::vector<std::string>();

		// Initialize user data
		dirPaths = new std::vector<std::string>();
		trkPaths = new std::vector<std::string>();

		// Initialize the screen
		std::string current = "Select a Track";
		GfuiTitleCreate(handle, "Select a Track", current.length());
		GfuiScreenAddBgImg(handle, "data/img/splash-qrdrv.png");

		// Positions - used to help with relative positioning
		Rect folderListRect = Rect(200, 350, 20, 60);
		Rect trkListRect = Rect(200, 350, 20 + folderListRect.width + 20, 60);

		// Add Labels
		GfuiLabelCreate(handle, "Track Configurations", GFUI_FONT_MEDIUM, folderListRect.x, folderListRect.y + folderListRect.height, GFUI_ALIGN_HL_VB, std::string("Track Configurations").length());
		GfuiLabelCreate(handle, "Previously Generated", GFUI_FONT_MEDIUM, trkListRect.x, trkListRect.y + trkListRect.height, GFUI_ALIGN_HL_VB, std::string("Previously Generated").length());

		// Add lists
		folderListID = GfuiScrollListCreate(handle, GFUI_FONT_MEDIUM, folderListRect.x, folderListRect.y, GFUI_ALIGN_HL_VT, folderListRect.width, folderListRect.height, GFUI_SB_LEFT, nullptr, PFolderSelectListClick);
		trkListID = GfuiScrollListCreate(handle, GFUI_FONT_MEDIUM, trkListRect.x, trkListRect.y, GFUI_ALIGN_HL_VT, trkListRect.width, trkListRect.height, GFUI_SB_RIGHT, nullptr, PTrackSelectListClick);

		// Add elements to lists
		std::string configDir = std::string(fileManager->GetCurrentDir()) + "tracks\\procedural\\";
		*dirs = fileManager->DirectoriesInDirectory(std::string(fileManager->GetCurrentDir()) + "tracks\\procedural\\");

		// Initialize user data arrays
		for (int i = 0; i < dirs->size(); i++)
				dirPaths->push_back(configDir + dirs->at(i));

		// Initialize list of track configurations
		int idx = 0;
		for (int i = 0; i < dirs->size(); i++)
		{
			if (dirs->at(i) != "." && dirs->at(i) != "..")
			{
				GfuiScrollListInsertElement(handle, folderListID, (char*)dirs->at(i).c_str(), idx, (char*)dirPaths->at(i).c_str());
				idx++;
			}
		}

		// Add Buttons
		GfuiButtonCreate(handle, "Back", GFUI_FONT_LARGE, trkListRect.x + trkListRect.width + 100, 300, GfuiFontWidth(GFUI_FONT_LARGE, "Back") + 20,
			GFUI_ALIGN_HC_VB, 0, p->prevScreen, PCancelTrackSelectMenu, nullptr, nullptr, nullptr);

		GfuiButtonCreate(handle, "Accept", GFUI_FONT_LARGE, trkListRect.x + trkListRect.width + 100, 350, GfuiFontWidth(GFUI_FONT_LARGE, "Accept") + 20,
			GFUI_ALIGN_HC_VB, 0, p, PAcceptTrackSelectMenu, nullptr, nullptr, nullptr);

		// Activate Screen
		GfuiScreenActivate(handle);
	}

	static void PFolderSelectListClick(void* /* dummy param */ )
	{
		selectedFolder = GfuiScrollListGetSelectedElement(handle, folderListID, (void**)&selectedFolderPath);
		PUpdateTrackList(nullptr);
	}

	static void PTrackSelectListClick(void* /* dummy param */)
	{
		selectedTrack = GfuiScrollListGetSelectedElement(handle, trkListID, (void**)&selectedTrackPath);
	}

	static void PCancelTrackSelectMenu(void* params)
	{
		GfuiScreenRelease(handle);
		PResetLoadScreen();

		if (params) {
			GfuiScreenActivate(params);
		}
	}

	static void PAcceptTrackSelectMenu(void* params)
	{
		// Set all parameters for later use
		PMenuParams* p = static_cast<PMenuParams*>(params);

		fileManager->SetTrackLoadState(PTrackLoadState(selectedTrack, std::string("tracks/procedural/" + selectedFolder + "/previousTracks/" + selectedTrack + "/"),
			selectedFolder, "tracks/procedural/" + selectedFolder + "/"));

		GfuiScreenRelease(handle);
		PResetLoadScreen();

		ReStartNewRace(params);
	}

	static void PUpdateTrackList(void* /* dummy param */)
	{
		// Construct the folder path to search for .TRK files
		std::string searchFolder = std::string(fileManager->GetCurrentDir()) + "tracks/procedural/" + selectedFolder + "/previousTracks/";

		// Find all TRK files in the directory
		trkDirs->clear();
		*trkDirs = fileManager->DirectoriesInDirectory(searchFolder);

		// Remove old elements
		void* tmp;
		char* str;
		while ((str = GfuiScrollListExtractElement(handle, trkListID, 0, (void**)&tmp)) != NULL) {
		}

		// Initialize user data array
		trkPaths->clear();
		for (int i = 0; i < trkDirs->size(); i++)
			trkPaths->push_back(searchFolder + trkDirs->at(i));

		// Add new elements
		nOfTracksInScrollList = trkDirs->size();
		for (int i = 0; i < nOfTracksInScrollList; i++)
			GfuiScrollListInsertElement(handle, trkListID, (char*)trkDirs->at(i).c_str(), i, (char*)trkPaths->at(i).c_str());
	}

	static void PResetLoadScreen()
	{
		// Delete vector pointers
		if (dirs)
			delete dirs;
		if (trkDirs)
			delete trkDirs;
		if (dirPaths)
			delete dirPaths;
		if (trkPaths)
			delete trkPaths;

		dirs = nullptr;
		trkDirs = nullptr;
		dirPaths = nullptr;
		trkPaths = nullptr;
		handle = nullptr;

		selectedTrack = "";
		selectedFolder = "";
		selectedTrackPath = "";
		selectedFolderPath = "";
	}
}