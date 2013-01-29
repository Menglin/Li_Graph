#include "Li_GameCtrl.h"
INT WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	srand((unsigned int)time(NULL)); // randomize timer

	Li_GameCtrl::initGame(hInstance, nCmdShow);
	Li_GameCtrl::go();
	Li_GameCtrl::shutdownGame();

	return 0;
}