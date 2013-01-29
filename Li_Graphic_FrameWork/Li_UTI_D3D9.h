#ifndef LI_UTI_D3D9_H
#define LI_UTI_D3D9_H

#include <d3d9.h>
#include <d3dx9.h>

// D3D9 Setup class //
class Li_UTI_D3D9
{
public:
	// Methods ////
	// sets up and initializes Direct3D
	HRESULT fn_InitD3D9(HWND hWnd, IDirect3DDevice9** ppDevice, D3DDEVTYPE deviceType, int width, int height, bool bIsWindowed);
	// closes Direct3D and releases memory
	HRESULT fn_CleanD3D9(LPDIRECT3DDEVICE9 pDevice);

	// Screen Capture
	HRESULT fn_ScreenGrab(LPDIRECT3DDEVICE9 pDevice, char *fileName);

	// Load Files From Zip
};

#endif