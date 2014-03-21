/*
	File: PMData.h
	Author: Henri Keeble
	Desc: Declarations for a class that holds performance measurement data.
*/

#ifndef _PM_DATA_H_
#define _PM_DATA_H_

#include <vector>
#include "../CarData.h"

namespace perfMeasurement
{
	/* Holds an individual set of performance measurement data. */
	class PMData
	{
		private:
			CarData data;
			tdble currentTime;
		public:
			PMData(CarData newData, tdble timeStamp);
			~PMData();
			
			/* Get car data stored by this object */
			CarData Data() const;
			
			/* Get time stamp stored by this object */
			tdble TimeStamp() const;
	};
}

#endif // _PM_DATA_H_