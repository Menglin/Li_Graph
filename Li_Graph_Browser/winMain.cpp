#include "Li_AppCtrl.h"
INT WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	srand((unsigned int)time(NULL)); // randomize timer

	Li_AppCtrl::initApp(hInstance, nCmdShow);
	Li_AppCtrl::go();
	Li_AppCtrl::shutdownApp();

	return 0;
}