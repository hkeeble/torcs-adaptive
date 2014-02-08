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
			driver.Update();

			Evaluate();

			timeOnLastUpdate = currentTime;
			cumulativeTime = 0.0f;
			OutputData(currentTime);
		}
	}

	void taPerfMeasurement::Clear()
	{
		driver.Clear();
	}

	float taPerfMeasurement::GetSkillEstimate()
	{
		return skillEstimate;
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
		
		GfOut("DRIVER\n");
		DATA_OUTPUT(WTD_CUR_STR,	driver.getDataFromTag(WTD_CUR_STR), WTD_CUR_STR_T);	
		DATA_OUTPUT(WTD_CUR_ACCEL,  driver.getDataFromTag(WTD_CUR_ACCEL), WTD_CUR_ACCEL_T);
		DATA_OUTPUT(WTD_CUR_BRK,	driver.getDataFromTag(WTD_CUR_BRK), WTD_CUR_BRK_T);
		DATA_OUTPUT(WTD_CUR_GR,		driver.getDataFromTag(WTD_CUR_GR), WTD_CUR_GR_T);
		DATA_OUTPUT(WTD_DMG,		driver.getDataFromTag(WTD_DMG), WTD_DMG_T);
		DATA_OUTPUT(WTD_NB_COLL,	driver.getDataFromTag(WTD_NB_COLL), WTD_NB_COLL_T);

		GfOut("PHYSICS\n");
		DATA_OUTPUT(WTP_CUR_SPD_X, driver.getDataFromTag(WTP_CUR_SPD_X), WTP_CUR_SPD_T);
		DATA_OUTPUT(WTP_CUR_SPD_Y, driver.getDataFromTag(WTP_CUR_SPD_Y), WTP_CUR_SPD_T);
		GfOut(("\tSpeed (km/h): " + dbleToStr(driver.curSpeed) + "\n").c_str());

		GfOut("MISC\n");
		DATA_OUTPUT(DIST_TO_STR_SEG, driver.getDataFromTag(DIST_TO_STR_SEG), DIST_TO_STR_SEG_T);
	}
}