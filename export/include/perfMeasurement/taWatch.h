/*
	File: drvTel.h
	Desc: Declarations for telemetric classes that track values of a driver or car physics data during a race.
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
#include "..\..\modules\simu\simuv2\carstruct.h"

namespace torcsAdaptive
{
	#define STATIC_CAST(data,type) static_cast<type*>(data) // Used to statically cast data

	/* Watch tags and types */
	#define WTD_NB_COLL			"Number of Collisions"
	#define WTD_NB_COLL_T		int

	#define WTD_DMG 			"Damage"
	#define WTD_DMG_T			int

	#define WTD_CUR_SEG			"Current Segment"
	#define WTD_CUR_SEG_T	 	tTrackSeg*
	
	#define WTD_CUR_ACCEL		"Current Acceleration"
	#define WTD_CUR_ACCEL_T		tdble

	#define WTD_CUR_STR			"Current Steer Command"
	#define WTD_CUR_STR_T		tdble

	#define WTD_CUR_BRK			"Current Brake Command"
	#define WTD_CUR_BRK_T		tdble

	#define WTD_CUR_GR			"Current Gear Selection"
	#define WTD_CUR_GR_T	 	int

	#define WTP_CUR_SPD_X		"Current X Speed"
	#define WTP_CUR_SPD_Y		"Current Y Speed"
	#define WTP_CUR_SPD			"Current Speed"
	#define WTP_CUR_SPD_T		tdble
	
	/* Generic Torcs-Adaptive Watch Class */
	class taWatch
	{
	protected:
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
		
		taWatch();
		virtual ~taWatch();
		virtual taWatch& operator=(const taWatch& param);
		taWatch(const taWatch& param);
		
		void* getDataFromTag(std::string tag);
		void Clear();
	public:
		friend class drvWatch;
	private:
		std::vector<watchItem> items;
	};
	
	/* Represents watch data for a single driver */
	class drvWatch : public taWatch
	{	
	protected:
		drvWatch();
		virtual ~drvWatch();
		virtual drvWatch& operator=(const drvWatch& param);
		drvWatch(const drvWatch& param);
		
		void SetCar(CarElt* car);
		const CarElt* GetCar() const;
		void Clear();
		
		void Update();
		
	public:
		friend class taPerfMeasurement;
	private:
		CarElt* car;
		double curSpeed;
	};
}
#endif // _DRVTEL_H_