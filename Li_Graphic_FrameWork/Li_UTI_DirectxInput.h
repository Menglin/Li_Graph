#ifndef LI_UTI_DIRECTXINPUT_H
#define LI_UTI_DIRECTXINPUT_H

#define DIRECTINPUT_VERSION  0x0800

#include <dinput.h>

/*
// The Macro input
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// can be used in the main loop any where like below

	if (KEY_DOWN(VK_ESCAPE))
		PostQuitMessage(0);

*/

class Li_DxInput
{
public:
	void fn_InitDxInput(HINSTANCE hInstance, HWND hWnd);	// sets up and initializes DirectInput
	void fn_DetectInput(void);								// gets the current input state
	void fn_CleanDxInput(void);								// closes DirectInput and releases memory

	BYTE m_KeyState[256];									// the storage for the key-information, 
	/* use as:
		if(m_KeyState[DIK_ESCAPE] & 0x80)
			presed
	*/
	DIMOUSESTATE	m_MouseState;							// the storage for the mouse-information
	/*
	LONG lX;	X Position offset
	LONG lY;	Y Position offset
	LONG lZ;	The movement of the mouse wheel.
						0, no change, negative values are downward movements and positive values are upward movements.
	BYTE rgbButtons[4];	This stores four values for buttons.
						Value 0 is the left mouse-button, 1 is the right button, 2 is the middle button,
						3 is the back button and 4 is the forward button. 
						These work exactly the way the 'keystate' array works.
	*/

protected:
	LPDIRECTINPUT8			m_dxInput;						// the pointer to our DirectInput interface
	LPDIRECTINPUTDEVICE8	m_dxKeyboard;					// the pointer to the keyboard device
	LPDIRECTINPUTDEVICE8	m_dxMouse;						// the pointer to the mouse device
};

#endif