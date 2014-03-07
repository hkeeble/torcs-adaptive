/*
	File: PLoadTrackMenu.h
	Author: Henri Keeble
	Desc: Contains functions to create a menu used to load tracks.
*/

#include <string>

#include "PLoadTrackScreen.h"
#include "PFileManager.h"

namespace procedural
{
	class PLoadTrackMenu
	{
	private:
		void* hndle;

		// Pointer to the file manager
		PFileManager* fileManager;

		const int wWidth;
		const int wHeight;
	public:
		PLoadTrackMenu() : hndle(nullptr), wWidth(800), wHeight(600), fileManager(PFileManager::Get()) { }
		~PLoadTrackMenu() { };

		/* Creates and shows the track selection menu */
		void Create(PTrackSelectMenu* ts);
	};
}