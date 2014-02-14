/*
	File: PMData.h
	Author: Henri Keeble
	Desc: Declarations for a class that holds a collection of performance data.
*/

#ifndef _PM_DATA_H_
#define _PM_DATA_H_

#include <vector>
#include "../CarData.h"

namespace perfMeasurement
{
	class PMData
	{
		private:
			CarData data;
			float currentTime;
		public:
			PMData(CarData newData, float timeStamp);
			~PMData();
			
			/* Get car data stored by this object */
			CarData Data() const;
			
			/* Get time stamp stored by this object */
			float TimeStamp() const;
	};

	class PMDataCollection
	{
		private:
			std::vector<PMData> data;
			int nOfDataSets;
			
			const int maxDataSets = 5;
		public:
			PMDataCollection();
			PMDataCollection(const PMDataCollection& param);
			PMDataCollection& operator=(const PMDataCollection& param);
			~PMDataCollection();
			
			/* Gets the number of elements of the current data collection */
			int Count() const;
			
			/* Add data to the collection */
			void AddData(const CarData& data, float timeStamp);
			
			/* Operator used to index data stored, index between 0 and 4 */
			const PMData& operator()(int index);

			/* Returns the latest data recording */
			const PMData& LatestData();

			/* Clears the data collection */
			void Clear();

			/* Gets the maximum number of data sets */
			int MaximumDataSets() const;
	};
}

#endif // _PM_DATA_H_