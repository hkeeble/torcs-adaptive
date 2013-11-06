/*
	File: seg.h
	Desc: header file for torcsAdaptive segment data
	Author: Henri Keeble
	Created: 06/11/2013
	Last Edit: 06/11/2013
*/
#ifndef _SEG_H_
#define _SEG_H_

#include "raceman.h"
#include <string>

namespace torcsAdaptive
{
	/* Represents segment data. */
	struct taSeg
	{
	public:
		taSeg(int raceInfo = TR_NORMAL, int type = TR_STR, int id = 0, int type2 = TR_MAIN, int style = TR_PLAN,
			tdble length = 10, tdble radius = 0, tdble radiusr = 0, tdble radiusl = 0, tdble arc = 0);
		~taSeg();

		std::string name;
			
		int	id;	

		int type;
				/*
					- TR_RGT
					- TR_LFT
					- TR_STR
				*/

		int type2;
				/*
				- TR_MAIN
				- TR_LSIDE
				- TR_RSIDE
				- TR_LBORDER
				- TR_RBORDER
				*/

		int style;
				/*
				- TR_PLAN
				- TR_CURB
				- TR_WALL
				- TR_FENCE
				- TR_PITBUILDING
				*/

		int raceInfo;
				/*
				- TR_NORMAL
				- TR_LAST
				- TR_START
				- TR_PITLANE
				- TR_SPEEDLIMIT
				- TR_PITENTRY
				- TR_PITEXIT
				- TR_PIT
				*/
		// Data
		tdble length;
		tdble lgfromstart;
			
		// For Corners
		tdble radius;
		tdble radiusr;
		tdble radiusl;
		tdble arc;	
			
		// 3D Data
		t3Dd center;		
		t3Dd vertex[4];
	};
}
		
#endif // _SEG_H_