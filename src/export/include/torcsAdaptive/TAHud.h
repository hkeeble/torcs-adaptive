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
	
	/* Used to represent a color, usable in the HUD by passing in the return of the Get() function */
	struct Color
	{
	private:
		float* color;
	public:
		Color()
		{ 
			color = new float[4];
			for (int i = 0; i < 4; i++)
				color[i] = 0.f;
		}

		Color(tdble R, tdble G, tdble B, tdble A)
		{
			color = new float[4];
			color[0] = R;
			color[1] = G;
			color[2] = B;
			color[3] = A;
		}

		~Color()
		{
			if (color)
				delete[] color;
		}

		Color& operator=(const Color& param)
		{
			if (&param == this)
				return *this;
			else
			{
				if (color)
					delete[] color;
				if (param.color)
				{
					for (int i = 0; i < 4; i++)
						color[i] = param.color[i];
				}
				else
				{
					color = new float[4];
					for(int i = 0; i < 4; i++)
						color[i] = 0.f;
				}

				return *this;
			}
		}

		Color(const Color& param)
		{
			for (int i = 0; i < 4; i++)
				color[i] = param.color[i];
		}

		float* Get()
		{
			return color;
		}
	};
	
	/* Used to encapsulte the HUD used by TORCS Adaptive */
	class TAHud
	{
	public:
		TAHud() : lFont(GFUI_FONT_BIG), mFont(GFUI_FONT_MEDIUM), sFont(GFUI_FONT_SMALL), wWidth(800), wHeight(600), isAdaptive(false),
			textColor(Color(1.f, 0.f, 0.f, 1.f)) { }
		~TAHud() { };
		
		/* Initialize the HUD */
		void Init(bool adaptiveRace);
		
		/* Render the HUD */
		void Render(tdble skillLevel);
		
	private:
		/* Constant values */
		Color textColor;
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