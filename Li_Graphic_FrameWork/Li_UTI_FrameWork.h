#ifndef LI_UTI_FW_H
#define LI_UTI_FW_H

#include "Li_UTI_Win32.h"
#include "Li_UTI_D3D9.h"
#include "Li_UTI_Draw.h"
#include "Li_UTI_DirectxInput.h"
#include "Li_UTI_CameraMan.h"
#include "Li_UTI_Sprite.h"

class Li_FW : public Li_Draw, public Li_Sprite_Interface
{
public:
	// Public Functions ////
	Li_FW();
	Li_FW(HINSTANCE hInstance,  int nCmdShow, char* szWndName = "Li FrameWork");
	~Li_FW();

	HRESULT fn_InitFW(HINSTANCE hInstance,	// [in] Application instance.
										int nCmdShow, char* szWndName = "Li FrameWork");

	HRESULT fn_ReleaseFW();

	virtual HRESULT fn_ctrl_initD3D();
	virtual HRESULT fn_ctrl_releaseD3D();

	// Message Loop
	HRESULT fn_MsgLoop();

protected:
	// Member Functions ////

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// d3d rendering time step, it is the outfit of the fn_ctrl_d3dRender()
	HRESULT fn_RenderFrame();

	// the entrence functions used to control the framework
	// control the graphics using overload function
	virtual void fn_ctrl_mainLogic() = 0;
	virtual void fn_ctrl_d3dRender() = 0;

	// tell the engine to update the frame
	static inline void fn_UpdateFrame(){s_UpdateGraphic = true;}
	void fn_ResetD3DDevice();

	// Input System
	// DX Input
	void fn_RegisterDxInput(HINSTANCE hInstance);
	void fn_ReleaseDxInput();

	// resize the window, not accessable from out side, only be triggered by WM_Size in WndProc
	static inline void fn_WindowResize(){s_ResetD3DDevice = true;}

	// move the camera
	void fn_MoveCamera();

	// Member Variables ////

	// The pointer to Direct3D device interface
	LPDIRECT3DDEVICE9	m_d3dDev;
	// Win32 API
	Li_UTI_Win32		*m_Win;
	// D3D API
	Li_UTI_D3D9			*m_D3d;

	// windows setting
	static int			m_WinWidth;
	static int			m_WinHeight;
	static bool			m_IsWindowed;

	// static members
	static bool			s_UpdateGraphic;

	// if the d3d device need to be reset
	static bool			s_ResetD3DDevice;

	// Mouse Position
	static D3DXVECTOR2	s_MousePosAbs;	// this is for the old winapi mouse, this is the absolute value
	static bool			s_isLBtnDown;	// L btn down for Winapi
	static bool			s_isRBtnDown;	// L btn down for Winapi

	// Directx Input, using dxinput register to register a pointer from outside, this one is only accessable from inside
	Li_DxInput	*m_DxInput;
	// POINT m_DxMousePosAbs;

	// for camera
	Li_CameraMan		*m_CamMan;

	// Cursor sprite
	Li_Sprite			m_cursor;
};

#endif