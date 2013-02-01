#ifndef LI_GAME_CTRL_H
#define LI_GAME_CTRL_H

#include <time.h>
#include "Li_GraphPlayer.h"
#include "../Li_ComnHeader/Li_L4WarningDisable.h"

namespace Li_GameCtrl
{
	static void initGame(HINSTANCE hInstance,  int nCmdShow)
	{
		mygp = new Li_GraphPlayer();
		mygp->fn_InitFW(hInstance, nCmdShow, "Li Graph Player");

		mygp->fn_LoadGraph("graph1.txt");
		mygp->fn_InitNodePos();
		mygp->fn_initStrategy();
	}

	static void go()
	{
		// Enter The Main Event Loop ////
		mygp->fn_MsgLoop(); // call fn_ctrl_mainLogic()
	}

	static void shutdownGame()
	{
		if (mygp != NULL)
			delete mygp;
	}
}

#endif