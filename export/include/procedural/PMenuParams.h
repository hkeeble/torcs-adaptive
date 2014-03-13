/*
	File: PMenuParams.h
	Author: Henri Keeble
	Desc: Parameter struct used in the procedural menu systems.
*/

#ifndef _P_MENU_PARAMS_H_
#define _P_MENU_PARAMS_H_

#include "PTrackLoadState.h"

namespace procedural
{
	/* Structure containing parameters to pass around, as callbacks are not encapsulated */
	struct PMenuParams
	{
		PMenuParams()
		{
			loadState = PTrackLoadState();
			prevScreen = nullptr;
			nextScreen = nullptr;
		}

		PTrackLoadState loadState;		/* The load state to pass on to the TAManager */
		void        *prevScreen;		/* Screen to go back to */
		void        *nextScreen;		/* Screen to go to after selection is made */
	};
}

#endif //_P_MENU_PARAMS_H_