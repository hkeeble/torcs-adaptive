/*
	File: PLoadTrackMenu.cpp
	Author: Henri Keeble
	Desc: Contains functions to create a menu used to select a procedural track configuration.
*/

#include "PMenu.h"
#include "tgfclient.h"
#include "tgf.h"
#include <string>

namespace procedural
{
	// Handle
	static void* handle;

	// File manager pointer
	static PFileManager* fileManager;

	// Static user data for this menu
	static std::string* selectedConfig;
	static int selectedIndex = 0;
	static std::string fPath;
	static std::vector<std::string>* configs;
	static std::vector<std::string>* configPaths;

	// Callback prototypes
	static void PAcceptConfig(void* params);
	static void PCancelConfig(void* params);
	static void PSelectConfig(void* /* dummy param */);

	// Reset function
	static void PResetConfigSelect();

	// Update function
	static void PUpdateConfigSelection();

	// Scroll list ID
	static int scID;

	// Label IDs
	static int mainWidthID;
	static int lsideWidthID;
	static int rsideWidthID;
	static int mainSurfaceID;
	static int lsideSurfaceID;
	static int rsideSurfaceID;

	// Path where configurations are found
	static std::string configPath = "";

	#define MAX_LABEL_LEN 100

	void PCreateTrackConfigSelectMenu(void* params)
	{
		// Cast parameters
		PMenuParams* p = (PMenuParams*)params;

		// Obtain file manager pointer
		fileManager = PFileManager::Get();

		// Intiailize user data
		selectedConfig = new std::string("");
		configs = new std::vector<std::string>();
		configPaths = new std::vector<std::string>();

		// Create handle
		handle = GfuiScreenCreate();

		// Create Title
		GfuiTitleCreate(handle, "Select Track Configuration", std::string("Select Track Configuration").length());
		
		// Set up background image
		GfuiScreenAddBgImg(handle, "data/img/splash-filesel.png");

		// Create Scroll List
		scID = GfuiScrollListCreate(handle, GFUI_FONT_MEDIUM, 20, 30, GFUI_ALIGN_HC_VC, 210, 400, GFUI_SB_RIGHT, nullptr, PSelectConfig);

		// Add configs to scroll list
		configPath = std::string(fileManager->GetCurrentDir()) + "tracks\\procedural\\";
		*configs = fileManager->DirectoriesInDirectory(configPath);

		// Obtain user data (paths of the actual configurations)
		int idx = 0;
		for (int i = 0; i < configs->size(); i++)
		{
			if (configs->at(i) != "." && configs->at(i) != "..")
				configPaths->push_back(configPath + configs->at(i) + "\\" + configs->at(i) + ".xml");
		}

		// Insert scroll list elements
		idx = 0;
		for (int i = 0; i < configs->size(); i++)
		{
			if (configs->at(i) != "." && configs->at(i) != "..")
			{
				GfuiScrollListInsertElement(handle, scID, (char*)configs->at(i).c_str(), idx, new int(idx));
				idx++;
			}
		}

		// Create buttons
		GfuiButtonCreate(handle, "Back", GFUI_FONT_LARGE, 310 + 100, 240, GfuiFontWidth(GFUI_FONT_LARGE, "Back") + 20,
			GFUI_ALIGN_HL_VC, 0, p, PCancelConfig, nullptr, nullptr, nullptr);

		GfuiButtonCreate(handle, "Accept", GFUI_FONT_LARGE, 310, 240, GfuiFontWidth(GFUI_FONT_LARGE, "Accept") + 20,
			GFUI_ALIGN_HL_VC, 0, p, PAcceptConfig, nullptr, nullptr, nullptr);

		// Display configuration information
		mainWidthID = GfuiLabelCreate(handle, "Main Width: - ", GFUI_FONT_MEDIUM, 310, 400, GFUI_ALIGN_HL_VC, MAX_LABEL_LEN);
		lsideWidthID = GfuiLabelCreate(handle, "Left Side Width: - ", GFUI_FONT_MEDIUM, 310, 380, GFUI_ALIGN_HL_VC, MAX_LABEL_LEN);
		rsideWidthID = GfuiLabelCreate(handle, "Right Side Width: - ", GFUI_FONT_MEDIUM, 310, 360, GFUI_ALIGN_HL_VC, MAX_LABEL_LEN);

		mainSurfaceID = GfuiLabelCreate(handle, "Main Surface: - ", GFUI_FONT_MEDIUM, 310, 340, GFUI_ALIGN_HL_VC, MAX_LABEL_LEN);
		lsideSurfaceID = GfuiLabelCreate(handle, "Left Side Surface: - ", GFUI_FONT_MEDIUM, 310, 320, GFUI_ALIGN_HL_VC, MAX_LABEL_LEN);
		rsideSurfaceID = GfuiLabelCreate(handle, "Right Side Surface: - ", GFUI_FONT_MEDIUM, 310, 300, GFUI_ALIGN_HL_VC, MAX_LABEL_LEN);

		GfuiScreenActivate(handle);
	}

	static void PAcceptConfig(void* params)
	{
		PMenuParams* p = (PMenuParams*)params;
		p->configName = *selectedConfig;
	
		GfuiScreenRelease(handle);
		PResetConfigSelect();

		if (p) {
			GfuiScreenActivate(p->nextScreen);
		}
	}

	static void PCancelConfig(void* params)
	{
		PMenuParams* p = (PMenuParams*)params;
		GfuiScreenRelease(handle);
		PResetConfigSelect();

		if (p) {
			GfuiScreenActivate(p->prevScreen);
		}
	}

	static void PSelectConfig(void* /* dummy param */)
	{ 
		*selectedConfig = GfuiScrollListGetSelectedElement(handle, scID, (void**)&selectedIndex);
		PUpdateConfigSelection();
	}

	static void PUpdateConfigSelection()
	{
		std::string fPath = "tracks\\procedural\\" + *selectedConfig + "\\" + *selectedConfig + ".xml";

		//void* fileHandle = GfParmReadFile(fPath.c_str(), GFPARM_RMODE_STD);

		//tdble mainWidth = GfParmGetNum(fileHandle, TRK_SECT_MAIN, TRK_ATT_WIDTH, (char*)nullptr, 0);
		//tdble lsideWidth = GfParmGetNum(fileHandle, TRK_SECT_MAIN, TRK_ATT_LSW, (char*)nullptr, 0);
		//tdble rsideWidth = GfParmGetNum(fileHandle, TRK_SECT_MAIN, TRK_ATT_RSW, (char*)nullptr, 0);

		//std::string mainSurface = GfParmGetStr(fileHandle, TRK_SECT_MAIN, TRK_ATT_SURF, " - ");
		//std::string lsideSurface = GfParmGetStr(fileHandle, TRK_SECT_MAIN, TRK_ATT_LSSURF, " - ");
		//std::string rsideSurface = GfParmGetStr(fileHandle, TRK_SECT_MAIN, TRK_ATT_RSSURF, " - ");

		//GfuiLabelSetText(handle, mainWidthID, std::string("Main Width: " + std::to_string(mainWidth)).c_str());
		//GfuiLabelSetText(handle, lsideWidthID, std::string("Left Side Width: " + std::to_string(lsideWidth)).c_str());
		//GfuiLabelSetText(handle, rsideWidthID, std::string("Right Side Width: " + std::to_string(rsideWidth)).c_str());

		//GfuiLabelSetText(handle, mainSurfaceID, std::string("Main Surface: " + mainSurface).c_str());
		//GfuiLabelSetText(handle, lsideSurfaceID, std::string("Left Side Surface: " + lsideSurface).c_str());
		//GfuiLabelSetText(handle, rsideSurfaceID, std::string("Right Side Surface: " + rsideSurface).c_str());

		// GfParmReleaseHandle(fileHandle);
	}

	static void PResetConfigSelect()
	{
		if (selectedConfig)
			delete selectedConfig;
		if (configs)
			delete configs;
		if (configPaths)
			delete configPaths;

		selectedConfig = nullptr;
		configs = nullptr;
		configPaths = nullptr;
		handle = nullptr;
	}
}