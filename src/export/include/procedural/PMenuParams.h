/**
 *  @file PMenuParams.h
 *	@author Henri Keeble
 *	@brief Declares a parameter struct used in the procedural menu systems.
*/

#ifndef _P_MENU_PARAMS_H_
#define _P_MENU_PARAMS_H_

#include "PTrackLoadState.h"
#include "PTrackManager.h"

namespace procedural
{
	/*! Structure containing parameters to pass around within the menu system, as callbacks are not encapsulated */
	struct PMenuParams
	{
		PMenuParams()
		{
			trkMngr = nullptr;
			prevScreen = nullptr;
			nextScreen = nullptr;
		}

		PTrackManager* trkMngr;			/*!< The track manager */
		void        *prevScreen;		/*!< Screen to go back to */
		void        *nextScreen;		/*!< Screen to go to after selection is made */
	};
}

#endif //_P_MENU_PARAMS_H_