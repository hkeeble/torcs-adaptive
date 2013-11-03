/*
	File: drvTel.h
	Desc: Declarations for telemetric classes that track values of a driver during a race.
	Author: Henri Keeble
	Created: 29/10/2013
	Last Edit: 02/10/2013
*/
#ifndef _DRVTEL_H_
#define _DRVTEL_H_

#include <iostream>
#include <string>
#include <vector>
#include "car.h"
#include "track.h"

namespace torcsAdaptive
{
	#define REINTERPRET_CAST(data,type) reinterpret_cast<type*>(data) // Used to statically cast data

	/* Watch tags and types */
	#define WT_NUMBER_OF_COLLISIONS 		"Number of Collisions"
	#define WT_NUMBER_OF_COLLISIONS_TYPE 	int

	#define WT_DAMAGE 						"Damage"
	#define WT_DAMAGE_TYPE 					int

	#define WT_CURRENT_SEG					"Current Segment"
	#define WT_CURRENT_SEG_TYPE 			tTrackSeg*

	#define WT_PREVIOUS_SEG					"Previous Segment"
	#define WT_PREVIOUS_SEG_TYPE			tTrackSeg*
	
	#define WT_CURRENT_ACCEL				"Current Acceleration"
	#define WT_CURRENT_ACCEL_TYPE			tdble

	#define WT_CURRENT_STEER				"Current Steer Command"
	#define WT_CURRENT_STEER_TYPE			tdble

	#define WT_CURRENT_BRAKE				"Current Brake Command"
	#define WT_CURRENT_BRAKE_TYPE			tdble

	#define WT_CURRENT_GEAR					"Current Gear Selection"
	#define WT_CURRENT_GEAR_TYPE 			int

	/* Represents watch data for a single driver */
	class drvWatch
	{
	private:
		/* Represents a single watch item */
		class watchItem
		{
		public:
			watchItem();
			virtual ~watchItem();
			virtual watchItem& operator=(const watchItem& param);
			watchItem(std::string tag, void* data);
			watchItem(const watchItem& param);
				
			std::string GetTag() const;
			void* GetData() const;
		private:
			std::string tag;
			void* data;
		};
		
	protected:
		drvWatch();
		virtual ~drvWatch();
		void* getDataFromTag(std::string tag);
		
		void SetCar(CarElt* car);
		const CarElt* GetCar() const;
		void Clear();
	public:
		friend class taPerfMeasurement;
	private:
		CarElt* car;
		std::vector<watchItem> items;
		
		// Additional Calculated Values
		tTrackSeg* previousSegment;
		double currentAvgSpeed;
	};
}
#endif // _DRVTEL_H_