/*
	File: PLoadTrackMenu.cpp
	Author: Henri Keeble
	Desc: Contains functions to create a menu used to load tracks.
*/

#include "PLoadTrackMenu.h"
#include "tgfclient.h"

namespace procedural
{
	/* Creates and displays the menu. Retrieves a pointer to a PTrackSelect menu, as this contains pointers to the previous and next screens that must be visited */
	void PLoadTrackMenu::Create(PTrackSelectMenu* ts)
	{
		// Create a screen handle
		void* hndle;
		hndle = GfuiScreenCreate();

		// Initialize the screen
		std::string current = "Select a Track";
		GfuiTitleCreate(hndle, "Select a Track", current.length());
		GfuiScreenAddBgImg(hndle, "data/img/splash-qrdrv.png");

		// Add list

		// Add Buttons (using ts previous for actions)
		GfuiButtonCreate(hndle, "Cancel", GFUI_FONT_MEDIUM, 0, 0, 10, GFUI_ALIGN_HC_VB, 0, nullptr, nullptr, nullptr, nullptr, nullptr);

		// Activate Screen
		GfuiScreenActivate(hndle);
	}
}