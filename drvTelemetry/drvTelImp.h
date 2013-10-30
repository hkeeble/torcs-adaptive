/*
	File: drvTel.h
	Desc: Interface declarations for telemetric classes that track values of a driver during a race.
	Author: Henri Keeble
	Created: 29/10/2013
	Last Edit: 29/10/2013
*/
#ifndef _DRVTELIMP_H_
#define _DRVTELIMP_H_

#include <iostream>
#include <string>
#include <vector>
#include "car.h"
#include "drvTel.h"

// Watch Tags
#define WT_NUMBER_OF_COLLISIONS "Number of Collisions"
#define WT_DAMAGE 				"Damage"
#define WT_CURRENT_SEG			"Current Segment"
#define WT_CURRENT_ACCEL		"Current Acceleration"
#define WT_CURRENT_STEER		"Current Steer Command"
#define WT_CURRENT_BRAKE		"Current Brake Command"
#define WT_CURRENT_GEAR			"Current Gear Selection"

/* Represents a single watch item */
class watchItemImp : public watchItem
{
private:
	std::string tag;
	void* data;
public:
	watchItemImp(std::string tag, void* data);
	~watchItemImp();

	watchItemImp(const watchItemImp& param);
	watchItemImp& operator=(const watchItemImp& param);

	std::string GetTag() const;
	void* GetData() const;
};

/* Represents watch data for a single driver */
class drvWatchImp : public drvWatch
{
private:
	std::vector<watchItemImp> items;
public:
	drvWatchImp(CarElt* car);
	~drvWatchImp();
	
	void* getDataFromTag(std::string tag);
};

/* Class used to handle driver telemetry */
class drvTelemetryImp : public drvWatch
{
private:
	drvWatchImp* driver;
public:
	drvTelemetryImp();
	~drvTelemetryImp();
	drvTelemetryImp(const drvTelemetryImp& param);
	drvTelemetryImp& operator=(const drvTelemetryImp& param);

	void SetDriver(CarElt* car);
	void Clear();
	void* getDataFromTag(std::string tag);
};

#endif // _DRVTELIMP_H_