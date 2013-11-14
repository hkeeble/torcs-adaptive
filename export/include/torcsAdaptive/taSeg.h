#ifndef _TA_SEG_H_
#define _TA_SEG_H_

#include <string>

namespace torcsAdaptive
{
/* Represents segment data for torcs-adaptive. */
	class taSeg
	{
	public:
		taSeg(int RaceInfo = 0, int Type = 0, int ID = 0, int Type2 = 0, int Style = 0,
			tdble Length = 10, tdble Radius = 0, tdble Radiusr = 0, tdble Radiusl = 0, tdble Arc = 0)
			: raceInfo(raceInfo), type(Type), id(ID), type2(Type2), style(Style), length(Length),
				radius(Radius), radiusr(Radiusr), radiusl(Radiusl), arc(Arc) { }
		~taSeg() { };

		std::string name;
				
		int	id;	
		int type;
		int type2;
		int style;
		int raceInfo;

		// Data
		tdble length;
				
		// For Corners
		tdble radius;
		tdble radiusr;
		tdble radiusl;
		tdble arc;	
	};
}
#endif // _TA_SEG_H_