#include "Li_UTI_DirectxInput.h"
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

// sets up and initializes DirectInput
void Li_DxInput::fn_InitDxInput(HINSTANCE hInstance, HWND hWnd)
{
	// create the DirectInput interface
	DirectInput8Create(hInstance,				// the handle to the application
						DIRECTINPUT_VERSION,	// the compatible version
						IID_IDirectInput8,		// the DirectInput interface version
						(void**)&m_dxInput,		// the pointer to the interface
						NULL);					// COM stuff, so we'll set it to NULL

	// create the keyboard device
	m_dxInput->CreateDevice(GUID_SysKeyboard,	// the default keyboard ID being used
							&m_dxKeyboard,		// the pointer to the device interface
							NULL);				// COM stuff, so we'll set it to NULL
	// create the mouse device
	m_dxInput->CreateDevice(GUID_SysMouse,
							&m_dxMouse,
							NULL);

	// set the data format to keyboard format
	m_dxKeyboard->SetDataFormat(&c_dfDIKeyboard);
	// set the data format to mouse format
	m_dxMouse->SetDataFormat(&c_dfDIMouse);

	// set the device cooperative level
	// DISCL_FOREGROUND: only read input when window is selected, instead of DISCL_BACkGROUND
	// DISCL_EXCLUSIVE do not allowed other app use the input device, instead of DISCL_NONEXCLUSIVE
	// DISCL_NOWINKEY will disable the windows logo key, such as alt+tab
	m_dxKeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	// set the device cooperative level
	// for mouse ecxlusive way will not display the cursur, the NonExclusive way will have it appear
	// for here, I want to use the win32api's mouse to get the abs pos, I also want to keep the ref pos from dx input
	// so I set it to NonExclusive, I deal with the cursur in the win porc.
	m_dxMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
}

// gets the current input state
void Li_DxInput::fn_DetectInput(void)
{
	// get access if we don't have it already
	m_dxKeyboard->Acquire();
	// get access if we don't have it already
	m_dxMouse->Acquire();

	// get the input data
	m_dxKeyboard->GetDeviceState(256, (LPVOID)m_KeyState);
	// get the input data
	m_dxMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_MouseState);
}

// closes DirectInput and releases memory
void Li_DxInput::fn_CleanDxInput(void)
{
	m_dxKeyboard->Unacquire();		// make sure the keyboard is unacquired
	m_dxMouse->Unacquire();			// make sure the mouse in unacquired
	m_dxInput->Release();			// close DirectInput before exiting
}