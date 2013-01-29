#ifndef LI_APP_CTRL_H
#define LI_APP_CTRL_H

#include <time.h>
#include "Li_GraphBrowser.h"
#include "../Li_ComnHeader/Li_L4WarningDisable.h"

namespace Li_AppCtrl
{
	static void initApp(HINSTANCE hInstance,  int nCmdShow)
	{
		mygb = new Li_GraphBrowser();
		mygb->fn_InitFW(hInstance, nCmdShow, "Li Graph Browser");

		mygb->fn_LoadGraph("graph1.txt");
		mygb->fn_InitNodePos();
	}

	static void go()
	{
		// Enter The Main Event Loop ////
		mygb->fn_MsgLoop();
	}

	static void shutdownApp()
	{
		if (mygb != NULL)
			delete mygb;
	}
}

#endif