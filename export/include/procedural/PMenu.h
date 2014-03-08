/*
	File: PMenu.h
	Author: Henri Keeble
	Desc: Contains functions for procedural library menus.
*/

#include <string>

#include "PLoadTrackScreen.h"
#include "PFileManager.h"
#include "taMath\taMath.h"

namespace procedural
{
#define NO_CONFIG_SET ""
#define NO_TRACK_LOAD_PATH ""

	// Structure containing parameters to pass around, as callbacks are not encapsulated
	struct PMenuParams
	{
		PMenuParams()
		{
			configName = NO_CONFIG_SET;
			trackLoadPath = NO_TRACK_LOAD_PATH;
			prevScreen = nullptr;
			nextScreen = nullptr;
		}

		std::string configName;		/* Selected configuration */
		std::string trackLoadPath;  /* Path of the track to load */
		void        *prevScreen;	/* Screen to go back to */
		void        *nextScreen;	/* Screen to go to after selection is made */
	};

	/* Creates and shows the procedural track selection menu */
	extern void PCreateTrackSelectMenu(void* ts);

	/* Creates and shows the menu for loading track configurations */
	extern void PCreateTrackConfigSelectMenu(void* ts);

	// Useful screen parameters
	extern const int wWidth;
	extern const int wHeight;
}