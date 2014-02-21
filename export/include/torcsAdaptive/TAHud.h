/*
	File: TAhHud.h
	Author: Henri Keeble
	Desc: Declares a class to handle the TORCS Adaptive HUD.
*/

#ifndef _TA_HUD_H_
#define _TA_HUD_H_

#include "tgfclient.h"
#include <string>

namespace torcsAdaptive
{
	#define TAHUD_ADAPT_TYPE_NAME "Adaptive Race"
	#define TAHUD_PROC_TYPE_NAME "Procedural Race"

	class TAHud
	{
	public:
		TAHud() : lFont(GFUI_FONT_BIG), mFont(GFUI_FONT_MEDIUM), sFont(GFUI_FONT_SMALL), wWidth(800), wHeight(600), isAdaptive(false) { }
		~TAHud();
		
		/* Initialize the HUD */
		void Init(bool adaptiveRace);
		
		/* Render the HUD */
		void Render(tdble skillLevel);
		
	private:
		/* Constant values */
		const float* const cRed		= new float[4] { 1.f, 0.f, 0.f, 1.f };
		const float* const cWhite	= new float[4] { 0.f, 0.f, 0.f, 1.f };
		const float* const cBlack	= new float[4] { 1.f, 1.f, 1.f, 1.f };
		const float* const cBlue	= new float[4] { 0.f, 0.f, 1.f, 1.f };
		const int lFont;
		const int mFont;
		const int sFont;
		const int wWidth;
		const int wHeight;
		
		// Is this an adaptive race?
		bool isAdaptive;
		std::string rTypeName;
	};
}

#endif // _TA_HUD_H_