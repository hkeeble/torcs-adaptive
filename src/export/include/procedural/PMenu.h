/**
 *	@file 	PMenu.h
 *	@author Henri Keeble
 *	@brief 	Contains functions for procedural library menus.
*/

#ifndef _P_MENU_H_
#define _P_MENU_H_

#include <string>

#include "PMenuParams.h"
#include "PFileManager.h"
#include "taMath\taMath.h"

namespace procedural
{
	//! Creates and shows the procedural track selection menu.
	/*!
		\param ts The parameters to pass into the menu.
	*/
	extern void PCreateTrackSelectMenu(void* ts);

	//! Creates and shows the menu for loading track configurations.
	/*!
		\param ts The parameters to pass into the menu.
	*/
	extern void PCreateTrackConfigSelectMenu(void* ts);

	//! Creates and shows the menu for saving a track.
	/*!
		\param ts The parameters to pass into the menu.
	*/
	extern void PCreateSaveMenu(void* ts);

	//! Creates and shows the menu for setting procedural track length.
	/*!
		\param ts The parameters to pass into the menu.
	*/
	extern void PCreateSetLengthMenu(void* ts);

	// Useful screen parameters
	extern const int wWidth;
	extern const int wHeight;
}

#endif // _P_MENU_H_