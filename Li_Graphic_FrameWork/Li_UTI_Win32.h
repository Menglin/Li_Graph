#ifndef LI_UTI_WIN32_H
#define LI_UTI_WIN32_H


// Includes ////
#include <windows.h>	// windows header
#include <windowsx.h>	// additional function for windows

// Defines ////
// make sure it is 32 bit program and do not include the MFC
#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN	// just say no to MFC
		#define BOOST_INTERPROCESS_WIN32_LEAN_AND_MEAN
	#endif	//#ifndef WIN32_LEAN_AND_MEAN
#endif		//#ifdef _WIN32

// Macros ////
// use direct input
//#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
//#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define NAMELEN 32	// windows name length

class Li_UTI_Win32
{
public:
	// Constructor ////
	Li_UTI_Win32(char* szWndName);
	// Destructor ////
	//~Li_UTI_Win32();

	// Load Parameters
	bool LoadInParameters(char* szFileName);

	// Initialize Windows
	bool InitWnd(HINSTANCE hInstance,	// [in] Application instance.
				WNDPROC WndProc,
				int nCmdShow);

	long long milliseconds_now();

	// Globals ////
	int		WINDOW_WIDTH;
	int		WINDOW_HEIGHT;
	bool	ISWINDOWED;
	int		iTimeDelta;					// sleep time delta
	char	szClsName[NAMELEN];			// window class name
	char	szAppName[NAMELEN];			// the application window name

	HWND hWndMain;						// globally track main window

	// RAW input device
	RAWINPUTDEVICE Rid[2];
};

#endif