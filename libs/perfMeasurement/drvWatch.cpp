/*
	File: drvTel.cpp
	Desc: Definitions for telemetric classes that track values of a driver during a race.
	Author: Henri Keeble
	Created: 29/10/2013
	Last Edit: 29/10/2013
*/

#include "perfMeasurement\drvWatch.h"
#include "tgf.h"

namespace torcsAdaptive
{
	/* ----- drvWatch Definitions ----- */
	drvWatch::drvWatch()
	{
		items = std::vector<watchItem>();
		car = NULL;
		previousSegment = NULL;
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

	void drvWatch::SetCar(CarElt* car)
	{
		Clear();
		
		this->car = car;

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

	const CarElt* drvWatch::GetCar() const
	{
		return (const CarElt*)car;
	}

	void drvWatch::Clear()
	{
		car = NULL;
		items.clear();
	}

	/* ----- watchItem Definitions ----- */
	drvWatch::watchItem::watchItem(std::string tag, void* data)
	{
		this->data = data;
		this->tag = tag;
	}

	drvWatch::watchItem::~watchItem()
	{

	}

	drvWatch::watchItem::watchItem(const watchItem& param)
	{
		this->data = param.data;
		this->tag = param.tag;
	}

	drvWatch::watchItem& drvWatch::watchItem::operator=(const watchItem& param)
	{
		if(this == &param)
			return *this;
		else
		{
			this->tag = param.tag;
			this->data = param.data;

			return *this;
		}
	}

	std::string drvWatch::watchItem::GetTag() const
	{
		return tag;
	}

	void* drvWatch::watchItem::GetData() const
	{
		return data;
	}
}