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
	
	struct Color
	{
	private:
		tdble r, g, b, a;
	public:
		Color() : r(0), g(0), b(0), a(0) { }
		Color(tdble R, tdble G, tdble B, tdble A) { r = R; g = G; b = B; a = A; }
		~Color() { }

		float* Get()
		{
			float* ret = new float[4];
			ret[0] = r;
			ret[1] = g;
			ret[2] = b;
			ret[3] = a;
			return ret;
		}
	};

	class TAHud
	{
	public:
		TAHud() : lFont(GFUI_FONT_BIG), mFont(GFUI_FONT_MEDIUM), sFont(GFUI_FONT_SMALL), wWidth(800), wHeight(600), isAdaptive(false),
			cRed(Color(1.f, 0.f, 0.f, 1.f)), cBlue(Color(0.f, 0.f, 1.f, 1.f)), cBlack(Color(0.f, 0.f, 0.f, 1.f)), cWhite(Color(1.f, 1.f, 1.f, 0.f)) { }
		~TAHud();
		
		/* Initialize the HUD */
		void Init(bool adaptiveRace);
		
		/* Render the HUD */
		void Render(tdble skillLevel);
		
	private:
		/* Constant values */
		Color cRed;
		Color cBlue;
		Color cBlack;
		Color cWhite;
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