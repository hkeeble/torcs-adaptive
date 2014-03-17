/*
	File: TAhHud.h
	Author: Henri Keeble
	Desc: Defines a class to handle the TORCS Adaptive HUD.
*/

#include "TAHud.h"
#include "perfMeasurement\PMDefs.h"

namespace torcsAdaptive
{
	void TAHud::Init(bool adaptiveRace)
	{
		isAdaptive = adaptiveRace;

		if (isAdaptive)
			rTypeName = TAHUD_ADAPT_TYPE_NAME;
		else
			rTypeName = TAHUD_PROC_TYPE_NAME;
	}

	void TAHud::Render(tdble skillLevel)
	{
		GfuiPrintString("TORCS Adaptive v1.0", textColor.Get(), sFont,
			wWidth - GfuiFontWidth(sFont, "TORCS Adaptive v1.0"), 10, GFUI_ALIGN_HL_VC);

		GfuiPrintString(rTypeName.c_str(), textColor.Get(), mFont, wWidth / 2, wHeight - GfuiFontHeight(mFont), GFUI_ALIGN_HC_VC);

		if (isAdaptive)
		{
			std::string out = "Skill Estimate: " + (skillLevel == NULL_SKILL_LEVEL ? "Gathering data..." : std::to_string(skillLevel));
			GfuiPrintString(out.c_str(), textColor.Get(), mFont, wWidth / 2, wHeight - GfuiFontHeight(mFont) * 2 - 5, GFUI_ALIGN_HC_VC);
		}
	}
}