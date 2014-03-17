/*
	File: PSetTrackLengthMenu.cpp
	Author: Henri Keeble
	Desc: Contains functions for creating a menu used to set the length of a procedural track.
*/

#include "PMenu.h"
#include "tgf.h"
#include "tgfclient.h"

namespace procedural
{
	static void* handle;

	static int editBoxID;
	static int errLabelID;

	static PFileManager* fManager;

	// Callback prototypes
	static void PAcceptCallback(void* params);
	static void PCancelCallback(void* params);
	static void PUpdateEditBox(void* params);

	void PCreateSetLengthMenu(void* ts)
	{
		handle = GfuiScreenCreate();

		fManager = PFileManager::Get();

		// Create Title
		GfuiTitleCreate(handle, "Set Length", std::string("Set Length").length());

		// Set up background image
		GfuiScreenAddBgImg(handle, "data/img/splash-filesel.png");

		// Create Edit Box
		GfuiLabelCreate(handle, "Track Length (km):", GFUI_FONT_MEDIUM, 310, 260, GFUI_ALIGN_HC_VC, std::string("Track Length (km):").length());
		editBoxID = GfuiEditboxCreate(handle, "1000", GFUI_FONT_MEDIUM, 265, 240, 100, 20, nullptr, nullptr, PUpdateEditBox);

		// Create error label
		errLabelID = GfuiLabelCreate(handle, " ", GFUI_FONT_MEDIUM, 30, 0, GFUI_ALIGN_HL_VB, 256);
		float col[4] = { 1.f, 0.f, 0.f, 1.f };
		GfuiLabelSetColor(handle, errLabelID, col);

		// Create buttons
		GfuiButtonCreate(handle, "Accept", GFUI_FONT_MEDIUM, 365, 200, 50, GFUI_ALIGN_HC_VC, 0, ts, PAcceptCallback, nullptr, nullptr, nullptr);
		GfuiButtonCreate(handle, "Cancel", GFUI_FONT_MEDIUM, 245, 200, 50, GFUI_ALIGN_HC_VC, 0, ts, PAcceptCallback, nullptr, nullptr, nullptr);

		GfuiScreenActivate(handle);
	}

	static void PAcceptCallback(void* params)
	{
		PMenuParams* p = static_cast<PMenuParams*>(params);

		std::string input = GfuiEditboxGetString(handle, editBoxID);
		int val = std::stoi(input, nullptr, 10);
		fManager->SetTrackLoadStateLength(val);
		GfuiScreenReplace(p->nextScreen);
	}

	static void PCancelCallback(void* params)
	{
		PMenuParams* p = static_cast<PMenuParams*>(params);
		GfuiScreenReplace(p->nextScreen);
	}

	static void PUpdateEditBox(void* params)
	{
		char* input = GfuiEditboxGetString(handle, editBoxID);
		int val = strtol(input, (char **)nullptr, 0);
		if (val == 0)
			GfuiEditboxSetString(handle, editBoxID, "1000");
	}
}