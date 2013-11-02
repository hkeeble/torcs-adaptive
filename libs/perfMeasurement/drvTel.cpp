/*
	File: drvTel.cpp
	Desc: Definitions for telemetric classes that track values of a driver during a race.
	Author: Henri Keeble
	Created: 29/10/2013
	Last Edit: 29/10/2013
*/

#include "perfMeasurement\drvTel.h"
#include "tgf.h"

/* ----- drvWatch Definitions ----- */
drvWatch::drvWatch()
{
	items = std::vector<watchItem>();
}

drvWatch::drvWatch(CarElt* car)
{
	items = std::vector<watchItem>();

	// Watch Pointers
	items.push_back(watchItem(WT_NUMBER_OF_COLLISIONS,	&car->priv.collision_state.collision_count));
	items.push_back(watchItem(WT_DAMAGE,				&car->priv.dammage));
	items.push_back(watchItem(WT_CURRENT_SEG,			car->priv.wheel[0].seg));
	items.push_back(watchItem(WT_CURRENT_ACCEL,			&car->ctrl.accelCmd));
	items.push_back(watchItem(WT_CURRENT_STEER,			&car->ctrl.steer));
	items.push_back(watchItem(WT_CURRENT_BRAKE,			&car->ctrl.brakeCmd));
	items.push_back(watchItem(WT_CURRENT_GEAR,			&car->ctrl.gear));
}

drvWatch::~drvWatch()
{
	items.clear();
}	

void* drvWatch::getDataFromTag(std::string tag)
{
	for(std::vector<watchItem>::iterator i = items.begin(); i != items.end(); i++)
	{
		if(i->GetTag() == tag)
			return i->GetData();
	}

	return NULL;
}

/* ----- watchItem Definitions ----- */
watchItem::watchItem(std::string tag, void* data)
{
	this->data = data;
	this->tag = tag;
}

watchItem::~watchItem()
{

}

watchItem::watchItem(const watchItem& param)
{

}

watchItem& watchItem::operator=(const watchItem& param)
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

std::string watchItem::GetTag() const
{
	return tag;
}

void* watchItem::GetData() const
{
	return data;
}

/* ----- winTelemetry Definitions ----- */
drvTelemetry::drvTelemetry()
{
	driver = NULL;
}

drvTelemetry::~drvTelemetry()
{

}

drvTelemetry::drvTelemetry(const drvTelemetry& param)
{
	driver = param.driver;
}

drvTelemetry& drvTelemetry::operator=(const drvTelemetry& param)
{
	if(this == &param)
		return *this;
	else
	{
		driver = param.driver;
		return *this;
	}
}

void drvTelemetry::SetDriver(CarElt* car)
{
	driver = new drvWatch(car);
}

void drvTelemetry::Clear()
{
	driver = NULL;
}

void* drvTelemetry::getDataFromTag(std::string tag)
{
	return driver->getDataFromTag(tag);
}