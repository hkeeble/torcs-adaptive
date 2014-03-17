/*
	File: PSaveTrackMenu.cpp
	Author: Henri Keeble
	Desc: Contains functions to create a menu used to save procedural tracks out to files.
*/

#include "PMenu.h"
#include "tgfclient.h"
#include "tgf.h"
#include <string>

namespace procedural
{
	static void* handle;

	// Callback prototypes
	void PAcceptCallback(void* params);
	void PCancelCallback(void* params);

	void PEditBoxFocus(void* params);

	static int editBoxID = 0;
	static int errLabelID = 0;

	void PCreateSaveMenu(void* params)
	{
		handle = GfuiScreenCreate();

		// Create Title
		GfuiTitleCreate(handle, "Save Track", std::string("Save Track").length());

		// Set up background image
		GfuiScreenAddBgImg(handle, "data/img/splash-filesel.png");

		// Add Edit Box
		GfuiLabelCreate(handle, "Track Name:", GFUI_FONT_MEDIUM, 295, 260, GFUI_ALIGN_HC_VC, std::string("Track Name:").length());
		editBoxID = GfuiEditboxCreate(handle, "", GFUI_FONT_MEDIUM, 265, 240, 100, 20, nullptr, PEditBoxFocus, nullptr);

		// Add error label
		errLabelID = GfuiLabelCreate(handle, " ", GFUI_FONT_MEDIUM, 40, 0, GFUI_ALIGN_HL_VB, 256);
		float col[4] = { 1.f, 0.f, 0.f, 1.f };
		GfuiLabelSetColor(handle, errLabelID, col);

		// Add Buttons
		GfuiButtonCreate(handle, "Save", GFUI_FONT_MEDIUM, 315, 220, 70, GFUI_ALIGN_HC_VC, 0, params, PAcceptCallback, nullptr, nullptr, nullptr);
		GfuiButtonCreate(handle, "Continue Without Saving", GFUI_FONT_MEDIUM, 315, 200, 220, GFUI_ALIGN_HC_VC, 0, params, PCancelCallback, nullptr, nullptr, nullptr);

		GfuiScreenActivate(handle);
	}

	void PAcceptCallback(void* params)
	{
		PMenuParams* p = static_cast<PMenuParams*>(params);

		std::string tname = GfuiEditboxGetString(handle, editBoxID);

		if (tname.length() <= 1)
		{
			GfuiLabelSetText(handle, errLabelID, "Please enter a track name with more than one character!");
			return;
		}
		else
		{
			std::vector<std::string> dirs = PFileManager::Get()->DirectoriesInDirectory(p->trkMngr->GetTrack()->GetConfigPath() + "previousTracks/");
			for (auto dir : dirs)
			{
				if (dir == tname)
				{
					GfuiLabelSetText(handle, errLabelID, "A track with this name already exists!");
					return;
				}
			}

			p->trkMngr->OutputCurrentTrack(tname);
			GfuiScreenReplace(p->nextScreen);
		}
	}

	void PEditBoxFocus(void* params)
	{
		GfuiLabelSetText(handle, errLabelID, " ");
	}

	void PCancelCallback(void* params)
	{
		PMenuParams* p = static_cast<PMenuParams*>(params);
		GfuiScreenReplace(p->nextScreen);
	}
}