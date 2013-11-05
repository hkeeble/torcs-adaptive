/*
	File: drvTel.cpp
	Desc: Definitions for telemetric classes that track values of a driver during a race.
	Author: Henri Keeble
	Created: 29/10/2013
	Last Edit: 29/10/2013
*/

#include "perfMeasurement\taWatch.h"
#include "tgf.h"

namespace torcsAdaptive
{
	/* ----- taWatch Definitions ----- */
	taWatch::taWatch()
	{
		items = std::vector<watchItem>();
	}

	taWatch::~taWatch()
	{
		items.clear();
	}	

	taWatch& taWatch::operator=(const taWatch& param)
	{
		if(this == &param)
			return *this;
		else
		{
			this->items = param.items;
			return *this;
		}
	}

	taWatch::taWatch(const taWatch& param)
	{
		Clear();
		items = std::vector<watchItem>(param.items);
	}

	void* taWatch::getDataFromTag(std::string tag)
	{
		for(std::vector<watchItem>::iterator i = items.begin(); i != items.end(); i++)
		{
			if(i->GetTag() == tag)
				return i->GetData();
		}

		return NULL;
	}

	void taWatch::Clear()
	{
		items.clear();
	}

	/* ----- watchItem Definitions ----- */
	taWatch::watchItem::watchItem(std::string tag, void* data)
	{
		this->data = data;
		this->tag = tag;
	}

	taWatch::watchItem::~watchItem()
	{

	}

	taWatch::watchItem::watchItem(const watchItem& param)
	{
		this->data = param.data;
		this->tag = param.tag;
	}

	taWatch::watchItem& taWatch::watchItem::operator=(const watchItem& param)
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

	std::string taWatch::watchItem::GetTag() const
	{
		return tag;
	}

	void* taWatch::watchItem::GetData() const
	{
		return data;
	}

	/* ----- drvWatch Definitions ----- */
	drvWatch::drvWatch() : taWatch()
	{
		car = NULL;
		curSpeed = 0;
	}

	drvWatch::~drvWatch()
	{
		taWatch::~taWatch();
	}

	drvWatch& drvWatch::operator=(const drvWatch& param)
	{
		if(this == &param)
			return *this;
		else
		{
			this->car = param.car;
			this->curSpeed = param.curSpeed;
			taWatch::operator=(param);
			return *this;
		}
	}
	
	drvWatch::drvWatch(const drvWatch& param) : taWatch(param)
	{
		this->car = param.car;
		this->curSpeed = param.curSpeed;
	}

	void drvWatch::SetCar(CarElt* car)
	{
		Clear();
		
		this->car = car;

		items = std::vector<watchItem>();

		// Watch Pointers
		// Driver
		items.push_back(watchItem(WTD_NB_COLL,	 &car->priv.collision_state.collision_count));
		items.push_back(watchItem(WTD_DMG,		 &car->priv.dammage));
		items.push_back(watchItem(WTD_CUR_SEG,	 car->priv.wheel[0].seg));
		items.push_back(watchItem(WTD_CUR_ACCEL, &car->ctrl.accelCmd));
		items.push_back(watchItem(WTD_CUR_STR,	 &car->ctrl.steer));
		items.push_back(watchItem(WTD_CUR_BRK,	 &car->ctrl.brakeCmd));
		items.push_back(watchItem(WTD_CUR_GR,	 &car->ctrl.gear));

		// Physics
		items.push_back(watchItem(WTP_CUR_SPD_X, &car->_speed_x));
		items.push_back(watchItem(WTP_CUR_SPD_Y, &car->_speed_y));
	}

	const CarElt* drvWatch::GetCar() const
	{
		return (const CarElt*)car;
	}

	void drvWatch::Update()
	{
		curSpeed = abs((car->_speed_x * 3.6));
	}

	void drvWatch::Clear()
	{
		taWatch::Clear();
		car = NULL;
	}
}