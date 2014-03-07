/*
	File: PLoadTrackScreen.h
	Author: Henri Keeble
	Desc: Structure used to represent the track loading screen.
*/

#ifndef _P_TRACK_SELECT_MENU_
#define _P_TRACK_SELECT_MENU_

/* Used to hold prev and next screens, and race manager parameters. Passed into create from TORCS in racescreens.cpp */
namespace procedural
{
	typedef struct
	{
		void        *param;		/* Race manager parameters where to set the selected track */
		void        *prevScreen;	/* Race manager screen to go back */
		void        *nextScreen;	/* Race manager screen to go after select */
	} PTrackSelectMenu;
}

#endif // _P_TRACK_SELECT_MENU_