/*
	File: PMenu.h
	Author: Henri Keeble
	Desc: Contains functions for procedural library menus.
*/

#ifndef _P_MENU_H_
#define _P_MENU_H_

#include <string>

#include "PMenuParams.h"
#include "PFileManager.h"
#include "taMath\taMath.h"

namespace procedural
{
	/* Creates and shows the procedural track selection menu */
	extern void PCreateTrackSelectMenu(void* ts);

	/* Creates and shows the menu for loading track configurations */
	extern void PCreateTrackConfigSelectMenu(void* ts);

	// Useful screen parameters
	extern const int wWidth;
	extern const int wHeight;
}

#endif // _P_MENU_H_