#include "Li_UTI_FrameWork.h"

// static members
int Li_FW::m_WinWidth;
int Li_FW::m_WinHeight;
bool Li_FW::m_IsWindowed;
bool Li_FW::s_UpdateGraphic		= true;
bool Li_FW::s_ResetD3DDevice	= false;
// for winapi mouse
D3DXVECTOR2 Li_FW::s_MousePosAbs;
bool Li_FW::s_isLBtnDown		= false;
bool Li_FW::s_isLClick			= false;
bool Li_FW::s_isLDBClick		= false;
bool Li_FW::s_isLRelease		= false;
bool Li_FW::s_isRBtnDown		= false;
bool Li_FW::s_isRClick			= false;
bool Li_FW::s_isRRelease		= false;

  /*------------------
 / Public Functions /
------------------*/

Li_FW::Li_FW()
{
	// Win32 API
	m_Win				= NULL;
	// D3D API
	m_D3d				= NULL;
	// Directx Input
	m_DxInput			= NULL;
	// the Camera Man
	m_CamMan			= NULL;
}

Li_FW::Li_FW(HINSTANCE hInstance, int nCmdShow, char* szWndName)
{
	m_Win				= NULL;
	m_D3d				= NULL;
	m_DxInput			= NULL;
	m_CamMan			= NULL;

	fn_InitFW(hInstance, nCmdShow, szWndName);
}

Li_FW::~Li_FW()
{
	fn_ReleaseFW();
}

HRESULT Li_FW::fn_InitFW(HINSTANCE hInstance,	// [in] Application instance.
						int nCmdShow, char* szWndName)
{
	// Initialize Win32 API
	if (m_Win == NULL)
	{
		m_Win = new Li_UTI_Win32(szWndName);
		if (!m_Win->InitWnd(hInstance, WndProc, nCmdShow))
		{
			MessageBox(0, "InitWnd() - FAILED", 0, 0);
			return FALSE;
		}
	}

	// the window size need to be got from the WM_SIZE from the win proc
	// because the titlebar takes some width and height
	// in winXP, it needs to be excately the number, otherwise, the d3ddevice.clear function may not work in XP
	//m_WinWidth		= m_Win->WINDOW_WIDTH;
	//m_WinHeight		= m_Win->WINDOW_HEIGHT;
	m_IsWindowed	= m_Win->ISWINDOWED;

	// initialize d3d device and the stuff depend on it
	fn_ctrl_initD3D();

	// register directx input
	fn_RegisterDxInput(hInstance);
	//GetCursorPos(&m_DxMousePosAbs); // bug

	// initialize CameraMan
	if (m_CamMan == NULL)
	{
		m_CamMan = new Li_CameraMan();
	}

	return S_OK;
}

HRESULT Li_FW::fn_ReleaseFW()
{
	// destroy CameraMan
	if (m_CamMan != NULL)
	{
		delete m_CamMan;
		m_CamMan = NULL;
	}

	// release directx input
	fn_ReleaseDxInput();

	// destroy d3d device and the stuff depend on it
	fn_ctrl_releaseD3D();
	
	if (m_Win != NULL)
	{
		delete m_Win;
		m_Win = NULL;
	}

	return S_OK;
}

// initialize d3d device relied stuff
HRESULT Li_FW::fn_ctrl_initD3D()
{
	// Initialize D3D API
	if (m_D3d == NULL)
	{
		m_D3d = new Li_UTI_D3D9();

		if (m_D3d->fn_InitD3D9(m_Win->hWndMain, &m_d3dDev, D3DDEVTYPE_HAL, m_WinWidth, m_WinHeight, m_IsWindowed) != S_OK)
		{
			MessageBox(0, "InitD3D() - FAILED", 0, 0);
			return FALSE;
		}
	}

	// initialize draw functions
	fn_InitDraw(m_d3dDev, (float)m_WinWidth, (float)m_WinHeight);

	// initialize sprite interface
	fn_InitSpriteInterface(m_d3dDev);
	// set cursor texture
	m_cursor.fn_setTexture(m_d3dDev, "../res/sdk_cursor.png");

	return S_OK;
}


HRESULT Li_FW::fn_ctrl_releaseD3D()
{
	// release cursor textrue
	m_cursor.fn_releaseTexture();
	// release sprite Interface
	fn_ReleaseSpriteInterface();

	// destroy draw functions
	fn_DestroyDraw();

	// close d3d and win32
	if (m_D3d != NULL)
	{
		m_D3d->fn_CleanD3D9(m_d3dDev);
		delete m_D3d;
		m_D3d = NULL;
	}

	return S_OK;
}

std::string Li_FW::openfilenameStr(char *filter, HWND owner)
{
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
	std::string fileNameStr;
	if ( GetOpenFileName(&ofn) )
		fileNameStr = fileName;
	return fileNameStr;
}

char* Li_FW::openfilename(char *filter, HWND owner)
{
	ShowCursor(true);

	OPENFILENAME ofn;
	static char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST; //OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
	
	if (!GetOpenFileName(&ofn))
	{
		strcpy_s(fileName, sizeof(fileName), "");
	}
	
	ShowCursor(false);
	
	// get only the name of the file, take the path and the extension name out

	// start from strlen -4 because the map file is .txt file,
	// the length of ".txt" is 4
	if (strlen(fileName) >=5 )
	{
		char tmpName[MAX_PATH];
		int j = 0;
		for (int i = strlen(fileName) - 5; i >= 0; i--)
		{
			// break at the first '\\'
			if (fileName[i] == '\\')
			{
				break;
			}

			tmpName[j] = fileName[i];
			j++;
		}
		// dont forget to put the '\0' in
		tmpName[j] = '\0';
		
		// because we got it through backward, so the tmpName needs to be reversed
		for (int k = 0; k < j/2; k++)
		{
			char tc;
			tc = tmpName[k];
			tmpName[k] = tmpName[j - k - 1];
			tmpName[j - k - 1] = tc;
		}

		strcpy_s(fileName, sizeof(fileName), tmpName);
	}

	return fileName;
}

// Main Message Loop
HRESULT Li_FW::fn_MsgLoop()
{
	MSG msg; // generic message
	ZeroMemory(&msg, sizeof(MSG));

	// test if this is a quit
	while (msg.message != WM_QUIT)
	{
		// Message Pump ////
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// translate any accelerator keys
			TranslateMessage(&msg);

			// send the message to window procdure
			DispatchMessage(&msg);
		}
		else
		{
			// detect input events
			if (m_DxInput != NULL)
			{
				m_DxInput->fn_DetectInput();

				// move cameraMan
				fn_MoveCamera();
			}

			// run main logic
			fn_ctrl_mainLogic();
			fn_keyListener();
			fn_mouseListener();

			// reset d3d device after resize window
			if (s_ResetD3DDevice)
				fn_ResetD3DDevice();

			// render graphics
			fn_RenderFrame();

			// Sleep, 1000 = 1 Sec.
			Sleep(m_Win->iTimeDelta);
		}
		// End Message Pump ////
	}

	// return to windows
	return msg.wParam;
}


  /*---------------------
 / Protected Functions /
---------------------*/

HRESULT Li_FW::fn_RenderFrame()
{
	// Only use Device methods if we have a valid device.
	// only render frame if it changed
	if (m_d3dDev && Li_FW::s_UpdateGraphic)
	{
		// clean back buffer
		// Instruct the device to set each pixel on the back buffer D3DCOLOR_XRGB - D3DCLEAR_TARGET: D3DCOLOR_XRGB(45, 50, 170)
		// And to set each pixel on the depth buffer to a value of 1.0 - D3DCLEAR_ZBUFFER: 1.0f.
		if (FAILED(m_d3dDev->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(125, 175, 225), 1.0f, 0 )))
			MessageBox(NULL, "Clear back buffer Failed", "Failure!", MB_OK);


		// start painting at back buffer
		if (SUCCEEDED(m_d3dDev->BeginScene()))
		{
			  ////////////////
			 // Paint Here //
			////////////////

			//// begin Sprite Interface ////
			if (SUCCEEDED(fn_spriteInterfaceBegin()))
			{
				// call outside rendering function
				fn_ctrl_d3dRender();

				// Draw FrameWork stuff at end, to make sure it under other drawing
				// draw cursor
				m_cursor.fn_drawSprite(m_dxSpriteInterface, D3DXVECTOR3(s_MousePosAbs.x, s_MousePosAbs.y, 0));
				// end draw FrameWork stuff

				//// end Sprite Interface ////
				fn_spriteInterfaceEnd();
			}

			// End Paint ///
			m_d3dDev->EndScene();
		}

		// Swap the back and front buffers.
		m_d3dDev->Present(NULL, NULL, NULL, NULL);

		// the update finished
		Li_FW::s_UpdateGraphic = false;
	} // end if

	return S_OK;
}

void Li_FW::fn_ResetD3DDevice()
{
	fn_ctrl_releaseD3D();
	fn_ctrl_initD3D();

	s_ResetD3DDevice = false;
}

// Input System
// DX Input
void Li_FW::fn_RegisterDxInput(HINSTANCE hInstance)
{
	m_DxInput = new Li_DxInput();
	m_DxInput->fn_InitDxInput(hInstance, m_Win->hWndMain);
}

void Li_FW::fn_ReleaseDxInput()
{
	m_DxInput->fn_CleanDxInput();
	delete m_DxInput;
	m_DxInput = NULL;
}

// move the Camera man
void Li_FW::fn_MoveCamera()
{
	//// right button down ////
	if (m_DxInput->m_MouseState.rgbButtons[1] &0x80)
	{
		m_CamMan->fn_MoveCamera(D3DXVECTOR2((float)m_DxInput->m_MouseState.lX, (float)m_DxInput->m_MouseState.lY));
		Li_FW::fn_UpdateFrame();
	}
	//// end right button down ////

	//// mouse wheel scale ////
	if (m_DxInput->m_MouseState.lZ > 0) // wheel upward
	{
		m_CamMan->fn_scaleUp(D3DXVECTOR2(s_MousePosAbs.x, s_MousePosAbs.y));
		Li_FW::fn_UpdateFrame();
	}
	else if(m_DxInput->m_MouseState.lZ < 0) // wheel downward
	{
		m_CamMan->fn_scaleDown(D3DXVECTOR2(s_MousePosAbs.x, s_MousePosAbs.y));
		Li_FW::fn_UpdateFrame();
	}
	//// end mouse wheel scale////
}