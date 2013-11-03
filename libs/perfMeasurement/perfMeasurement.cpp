/*
	File: perfMeasurementImp.h
	Desc: Definitions for a performance measurement class that uses telemetry to measure player performance.
	Author: Henri Keeble
	Created: 02/10/2013
	Last Edit: 02/10/2013
*/

#include "perfMeasurement\perfMeasurement.h"
#include "Util.h"

namespace torcsAdaptive
{
	taPerfMeasurement::taPerfMeasurement()
	{
		skillEstimate = 0;
		driver = drvWatch();
		cumulativeTime = 0.0f;
		timeOnLastUpdate = 0.0f;
	}

	taPerfMeasurement::~taPerfMeasurement()
	{

	}

	taPerfMeasurement::taPerfMeasurement(const taPerfMeasurement& param)
	{
		skillEstimate = param.skillEstimate;
		driver = param.driver;
	}

	taPerfMeasurement& taPerfMeasurement::operator=(const taPerfMeasurement& param)
	{
		if(&param == this)
			return *this;
		else
		{
			skillEstimate = param.skillEstimate;
			driver = param.driver;
			return *this;
		}
	}

	
	void taPerfMeasurement::SetDriver(CarElt* car)
	{
		driver.SetCar(car);
	}

	const CarElt* taPerfMeasurement::GetCar() const
	{
		return driver.GetCar();
	}

	void* taPerfMeasurement::getDataFromTag(std::string tag)
	{
		return driver.getDataFromTag(tag);
	}

	/* Update Performance Measurement, Evaluate new skill level if neccesary
		@param deltaTimeIncrement	Delta Time since last update call
		@param currentTime			Total current race time
	*/
	void taPerfMeasurement::Update(double deltaTimeIncrement, double currentTime)
	{
		cumulativeTime += deltaTimeIncrement;

		if(cumulativeTime >= PERFMEASURE_UPDATE_INTERVAL)
		{
			CalculateAvgSpeed(currentTime);
			Evaluate();

			timeOnLastUpdate = currentTime;
			//driver.previousSegment = *REINTERPRET_CAST(driver.getDataFromTag(WT_CURRENT_SEG), WT_CURRENT_SEG_TYPE);
			cumulativeTime = 0.0f;
			OutputData(currentTime);
		}
	}

	void taPerfMeasurement::Clear()
	{
		driver.Clear();
	}

	int taPerfMeasurement::GetSkillEstimate()
	{
		return skillEstimate;
	}

	void taPerfMeasurement::CalculateAvgSpeed(double currentTime)
	{
		if(driver.previousSegment)
		{
			tTrackSeg* curSeg = *REINTERPRET_CAST(driver.getDataFromTag(WT_CURRENT_SEG), WT_CURRENT_SEG_TYPE);
			int distance = 0;
			while(curSeg != driver.previousSegment)
			{
				distance += curSeg->length;
				curSeg = curSeg->prev;
			}
			
			driver.currentAvgSpeed = distance/(currentTime-timeOnLastUpdate);
		}
	}

	void taPerfMeasurement::Evaluate()
	{
		int eval = 0;
		// ENTER EVALUATION FUNCTION HERE
		skillEstimate = eval;
	}

	void taPerfMeasurement::OutputData(double currentTime)
	{
		GfOut((" ----- TA PerfMeasurement Output at " + dbleToStr(currentTime) + " ----- \n").c_str());
		
		GfOut("CONTROLS\n");
		DATA_OUTPUT("\t" WT_CURRENT_STEER, driver.getDataFromTag(WT_CURRENT_STEER), WT_CURRENT_STEER_TYPE);	
		DATA_OUTPUT("\t" WT_CURRENT_ACCEL, driver.getDataFromTag(WT_CURRENT_ACCEL), WT_CURRENT_ACCEL_TYPE);
		DATA_OUTPUT("\t" WT_CURRENT_BRAKE, driver.getDataFromTag(WT_CURRENT_BRAKE), WT_CURRENT_BRAKE_TYPE);
		DATA_OUTPUT("\t" WT_CURRENT_GEAR, driver.getDataFromTag(WT_CURRENT_GEAR), WT_CURRENT_GEAR_TYPE);
		
		GfOut("QUALITATIVE\n");
		DATA_OUTPUT(WT_DAMAGE, driver.getDataFromTag(WT_DAMAGE), WT_DAMAGE_TYPE);
		DATA_OUTPUT(WT_NUMBER_OF_COLLISIONS, driver.getDataFromTag(WT_NUMBER_OF_COLLISIONS), WT_NUMBER_OF_COLLISIONS_TYPE);
	}
}