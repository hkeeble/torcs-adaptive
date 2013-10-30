/*
	File: drvTel.cpp
	Desc: Definitions for telemetric classes that track values of a driver during a race.
	Author: Henri Keeble
	Created: 29/10/2013
	Last Edit: 29/10/2013
*/

#include "drvTelImp.h"
#include "tgf.h"

/* ----- drvWatch Definitions ----- */

drvWatchImp::drvWatchImp(CarElt* car)
{
	items = std::vector<watchItemImp>();

	// Watch Pointers
	items.push_back(watchItemImp(WT_NUMBER_OF_COLLISIONS,	&car->priv.collision_state.collision_count));
	items.push_back(watchItemImp(WT_DAMAGE,					&car->priv.dammage));
	items.push_back(watchItemImp(WT_CURRENT_SEG,			car->priv.wheel[0].seg));
	items.push_back(watchItemImp(WT_CURRENT_ACCEL,			&car->ctrl.accelCmd));
	items.push_back(watchItemImp(WT_CURRENT_STEER,			&car->ctrl.steer));
	items.push_back(watchItemImp(WT_CURRENT_BRAKE,			&car->ctrl.brakeCmd));
	items.push_back(watchItemImp(WT_CURRENT_GEAR,			&car->ctrl.gear));
}

drvWatchImp::~drvWatchImp()
{
	items.clear();
}	

void* drvWatchImp::getDataFromTag(std::string tag)
{
	for(std::vector<watchItemImp>::iterator i = items.begin(); i != items.end(); i++)
	{
		if(i->GetTag() == tag)
			return i->GetData();
	}

	return NULL;
}

/* ----- watchItem Definitions ----- */
watchItemImp::watchItemImp(std::string tag, void* data)
{
	this->data = data;
	this->tag = tag;
}

watchItemImp::~watchItemImp()
{

}

watchItemImp::watchItemImp(const watchItemImp& param)
{

}

watchItemImp& watchItemImp::operator=(const watchItemImp& param)
{
	if(this == &param)
		return *this;
	else
	{
		this->data = param.data;
		this->tag = param.tag;
		return *this;
	}
}

std::string watchItemImp::GetTag() const
{
	return tag;
}

void* watchItemImp::GetData() const
{
	return data;
}

/* ----- winTelemetry Definitions ----- */
drvTelemetryImp::drvTelemetryImp()
{
	driver = NULL;
}

drvTelemetryImp::~drvTelemetryImp()
{

}

drvTelemetryImp::drvTelemetryImp(const drvTelemetryImp& param)
{
	driver = param.driver;
}

drvTelemetryImp& drvTelemetryImp::operator=(const drvTelemetryImp& param)
{
	if(this == &param)
		return *this;
	else
	{
		driver = param.driver;
		return *this;
	}
}

void drvTelemetryImp::SetDriver(CarElt* car)
{
	driver = new drvWatchImp(car);
}

void drvTelemetryImp::Clear()
{
	driver = NULL;
}

void* drvTelemetryImp::getDataFromTag(std::string tag)
{
	return driver->getDataFromTag(tag);
}