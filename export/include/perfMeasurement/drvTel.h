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

// Watch Tags
#define WT_NUMBER_OF_COLLISIONS "Number of Collisions"
#define WT_DAMAGE 				"Damage"
#define WT_CURRENT_SEG			"Current Segment"
#define WT_CURRENT_ACCEL		"Current Acceleration"
#define WT_CURRENT_STEER		"Current Steer Command"
#define WT_CURRENT_BRAKE		"Current Brake Command"
#define WT_CURRENT_GEAR			"Current Gear Selection"

/* Represents a single watch item */
class watchItem
{
public:
	watchItem();
	watchItem(std::string tag, void* data);
	virtual ~watchItem();

	watchItem(const watchItem& param);
	watchItem& operator=(const watchItem& param);

	std::string GetTag() const;
	void* GetData() const;
private:
	std::string tag;
	void* data;
};

/* Represents watch data for a single driver */
class drvWatch
{
public:
	drvWatch();
	drvWatch(CarElt* car);
	virtual ~drvWatch();
	
	void* getDataFromTag(std::string tag);
private:
	std::vector<watchItem> items;
};

/* Class used to handle driver telemetry */
class drvTelemetry
{
public:
	drvTelemetry();
	virtual ~drvTelemetry();
	
	drvTelemetry(const drvTelemetry& param);
	drvTelemetry& operator=(const drvTelemetry& param);

	void SetDriver(CarElt* car);
	void Clear();
	void* getDataFromTag(std::string tag);
private:
	drvWatch* driver;
};

#endif // _DRVTEL_H_