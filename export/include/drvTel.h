/*
	File: drvTel.h
	Desc: Interface declarations for telemetric classes that track values of a driver during a race.
	Author: Henri Keeble
	Created: 29/10/2013
	Last Edit: 29/10/2013
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

#ifdef PERFMEASUREMENT_EXPORT
#define PERFMEASUREMENT_API __declspec(dllexport)
#else
#define PERFMEASUREMENT_API __declspec(dllimport)
#endif // PERFMEASUREMENT_EXPORT

/* Represents a single watch item */
class watchItem
{
protected:
	watchItem() { };
public:
	virtual ~watchItem() { };

	watchItem(const watchItem& param) { };
	virtual watchItem& operator=(const watchItem& param) { return *this; };

	virtual std::string GetTag() const = 0;
	virtual void* GetData() const = 0;
};

/* Represents watch data for a single driver */
class drvWatch
{
protected:
	drvWatch() { };
public:
	virtual ~drvWatch() { };
	
	virtual void* getDataFromTag(std::string tag) = 0;
};

/* Class used to handle driver telemetry */
class drvTelemetry
{
protected:
	drvTelemetry() { };
public:
	virtual ~drvTelemetry() { };
	
	drvTelemetry(const drvTelemetry& param) { };
	virtual drvTelemetry& operator=(const drvTelemetry& param) { };

	virtual void SetDriver(CarElt* car) = 0;
	virtual void Clear() = 0;
	virtual void* getDataFromTag(std::string tag) = 0;
};

// Factory Function Declaration
extern "C" // Prevents Name Mangling, exports as a C function
	PERFMEASUREMENT_API // Define Export or Import (dependent on command line input from MSVC)
	drvTelemetry* 
	APIENTRY // Defined as _stdcall in WinDef.h - to do with calling conventions
	CreateDrvTelemetry();

#endif // _DRVTEL_H_