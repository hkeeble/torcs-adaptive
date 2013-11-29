#ifndef _TA_SEG_H_
#define _TA_SEG_H_

#include <string>

namespace torcsAdaptive
{
	class TaSegFactory;

	/* Represents segment data for torcs-adaptive. */
	class taSeg
	{
	private:
		taSeg();
	public:	
		friend class TaSegFactory;

		virtual ~taSeg();

		int	id;	
		int type;
		int type2;
		int style;
		int raceInfo;

		// Data
		float length;
				
		// For Corners
		float radius;
		float radiusr;
		float radiusl;
		float arc;	
	};

	enum TaCornerType
	{
		TaRight = 1,
		TaLeft = 2
	};

	/* Factory for segments, singleton class */
	class TaSegFactory
	{
	private:
		TaSegFactory();
		virtual ~TaSegFactory();
		static TaSegFactory* instance;
	public:
		static TaSegFactory* GetInstance();

		// Creates a straight segment
		taSeg CreateSegStr(int id, float length);

		// Creates a corner segment
		taSeg CreateSegCnr(int id, TaCornerType cType, float radius, float radiusr, float radiusl, float arc);
	};
}
#endif // _TA_SEG_H_