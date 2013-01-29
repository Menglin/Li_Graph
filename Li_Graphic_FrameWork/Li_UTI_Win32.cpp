#include <fstream>
#include "Li_UTI_Win32.h"

using namespace std;

  ///////////////////////
 // Windows API Setup //
///////////////////////
// Construstor
Li_UTI_Win32::Li_UTI_Win32(char* szWndName)
{
	// 读取参数
	if(!LoadInParameters("params.ini"))
	{
		MessageBox(NULL, "Cannot find ini file!", "Error", 0);
	}

	strcpy_s(szClsName, "LiWndCls");
	strcpy_s(szAppName, szWndName);

	// 初始化 Raw input
	// Keyboard
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x06;
	Rid[0].dwFlags = 0;
	Rid[0].hwndTarget=NULL;

	// Mouse
	Rid[1].usUsagePage = 0x01;
	Rid[1].usUsage = 0x02;
	Rid[1].dwFlags = 0;
	Rid[1].hwndTarget=NULL;

	if (RegisterRawInputDevices(Rid,2,sizeof(RAWINPUTDEVICE))==FALSE)
	{
		//registration failed. Call GetLastError for the cause of the error
		MessageBox(NULL, "Failed to register Raw Input Device!", "Error", 0);
	}
}

// Load Parameters
bool Li_UTI_Win32::LoadInParameters(char *szFileName)
{
	ifstream grab(szFileName);
	//check file exists
	if (!grab)
	{
		return false;
	}

	//load in from the file
	char ParamDescription[32];

	grab >> ParamDescription;
	grab >> WINDOW_WIDTH;
	grab >> ParamDescription;
	grab >> WINDOW_HEIGHT;
	grab >> ParamDescription;
	grab >> ISWINDOWED;
	grab >> ParamDescription;
	grab >> iTimeDelta;
	// this is the refreash rate, we transmit it to time delta. 1000ms = 1s
	if (iTimeDelta != 0)
		iTimeDelta = 1000 / iTimeDelta;

	grab.close();

	return true;
}

// Initialize Windows
bool Li_UTI_Win32::InitWnd(
	HINSTANCE hInstance,	// [in] Application instance.
	WNDPROC WndProc,
	int nCmdShow)
{
	// Defines
	WNDCLASSEX	wincls;		// windows class structure
	HWND		hWnd;		// generic window handle

	// first fill in the window class structure
	wincls.cbClsExtra		= 0;
	wincls.cbSize			= sizeof(WNDCLASSEX);
	wincls.cbWndExtra		= 0;
	wincls.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wincls.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wincls.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wincls.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);
	wincls.hInstance		= hInstance;
	wincls.lpfnWndProc		= (WNDPROC)WndProc;
	wincls.lpszClassName	= szClsName;
	wincls.lpszMenuName		= NULL;
	wincls.style			= CS_DBLCLKS | CS_OWNDC |
							CS_HREDRAW | CS_VREDRAW;

	// register the window class
	if (!RegisterClassEx(&wincls))
	{
		MessageBox(NULL, "Class Registration Failed!", "ERROR", MB_OK);

		// exit the application
		return false;
	}

	// create the window
	if (ISWINDOWED)
	{
		hWnd = CreateWindowEx(NULL,				// dwExStyle
			szClsName,							// name of the window class
			szAppName,							// title of the window
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,	// window style
			25,25,								// position of the window (x, y)
			WINDOW_WIDTH,						// width of the window
			WINDOW_HEIGHT,						// height of the window
			NULL,								// parrent handle
			NULL,								// menu handle
			hInstance,							// application handle
			NULL);								// used with multiple windows, NULL
	}
	else
	{
		hWnd = CreateWindowEx(NULL,				// dwExStyle
			szClsName,							// name of the window class
			szAppName,							// title of the window
			WS_EX_TOPMOST | WS_POPUP,			// window style
			0,0,								// position of the window (x, y)
			WINDOW_WIDTH,						// width of the window
			WINDOW_HEIGHT,						// height of the window
			NULL,								// parrent handle
			NULL,								// menu handle
			hInstance,							// application handle
			NULL);								// used with multiple windows, NULL
	}

	if (!hWnd)
	{
		MessageBox(NULL, "Class Creation Failed!", "ERROR", MB_OK);

		// exit the application
		return false;
	}

	// save main window handle
	hWndMain = hWnd;
	
	// make the window visible
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return true;
}