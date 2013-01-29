#include "Li_UTI_D3D9.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

  ////////////////
 // D3D9 Setup //
////////////////
HRESULT Li_UTI_D3D9::fn_InitD3D9(HWND hWnd, IDirect3DDevice9** ppDevice, D3DDEVTYPE deviceType, int width, int height, bool bIsWindowed)
{
	// Step 1, Create IDirect3D9 Object
	LPDIRECT3D9 d3d; // The pointer to Direct3D interface
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (NULL == d3d)
	{
		MessageBox(NULL, "Direct3DCreate9() - FAILED!", "Failure!", MB_OK);
		// return false
		return E_FAIL;
	}

	// Step 2: Check for hardware vp(Vertex Processing).
	D3DCAPS9 caps;							// Device Caps
	d3d->GetDeviceCaps(D3DADAPTER_DEFAULT,	// Denotes primary display adapter
					deviceType,				// Specifies the device type, usually D3DDEVTYPE_HAL
					&caps);					// Return filled D3DCAPS9 structure that contains the capabilities of the primary display adapter

	// check if hardware vertex processing can be used
	int vp = 0;	// Vertex Processing
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		// use hardware vertex processing
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		// use software vertex processing
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
	D3DPRESENT_PARAMETERS d3dpp;					// Structure that hold various device information
	ZeroMemory(&d3dpp, sizeof(d3dpp));				// Clear out the struct for use

	d3dpp.SwapEffect	= D3DSWAPEFFECT_DISCARD;	// Type of swap chain - discard old frames
	d3dpp.hDeviceWindow	= hWnd;						// Handle of the window to be used by Direct3D

	if (bIsWindowed)
	{
		d3dpp.Windowed			= TRUE;				// Windowed or full screen
	}
	else
	{
		d3dpp.Windowed			= FALSE;			// Windowed or full screen
		d3dpp.BackBufferFormat	= D3DFMT_X8R8G8B8;	// set the back buffer format to 32-bit
		d3dpp.BackBufferWidth	= width;			// set the width of the buffer
		d3dpp.BackBufferHeight	= height;			// set the height of the buffer
	}

	// setup z-buffer
	d3dpp.EnableAutoDepthStencil = TRUE;			// automatically run the z-buffer for us
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;	// 24-bit pixel format for the z-buffer

	// Step 4: Create a device class using this information and information from the d3dpp stuct
	if (FAILED(d3d->CreateDevice(
		D3DADAPTER_DEFAULT,							// Graphics adapter or vedio card
		deviceType,									// DeviceType HAL/REF/...
		hWnd,										// Handle of the window
		vp,											// Vertex processing method
		&d3dpp,										// Pointer to the d3dpp struct
		ppDevice)))									// Pointer to the graphics device interface
	{
		MessageBox(NULL, "CreateDevice() - FAILED! Now using 16-bit Z-Buffer try again", "Failure!", MB_OK);
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;	// 16-bit pixel format for the z-buffer
		if (FAILED(d3d->CreateDevice(
		D3DADAPTER_DEFAULT,							// Graphics adapter or vedio card
		deviceType,									// DeviceType HAL/REF/...
		hWnd,										// Handle of the window
		vp,											// Vertex processing method
		&d3dpp,										// Pointer to the d3dpp struct
		ppDevice)))									// Pointer to the graphics device interface
		{
			MessageBox(NULL, "CreateDevice() - FAILED! Again!", "Failure!", MB_OK);
			// Release IDirect3D9 Object;
			d3d->Release();
			d3d = NULL;
			// return false
			return E_FAIL;
		}
	}

	// Release IDirect3D9 Object;
	d3d->Release();
	d3d = NULL;

	// Set Render State ////
	// draw double side
	(*ppDevice)->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// turn on the z-buffer
	(*ppDevice)->SetRenderState(D3DRS_ZENABLE, TRUE);
	(*ppDevice)->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	(*ppDevice)->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	// turn on the 3D lighting
	(*ppDevice)->SetRenderState(D3DRS_LIGHTING, TRUE);
	// ambient light
	(*ppDevice)->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));
	// the heigh light effect
	(*ppDevice)->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	// fix the problem that the normal scaling when object scaling that make the object brighter as it smaller
	(*ppDevice)->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// view Port and Clipping, do not draw more than it needed, save gpu time
	D3DVIEWPORT9 viewPort;
	viewPort.X			= 0;
	viewPort.Y			= 0;
	viewPort.Width		= width;
	viewPort.Height		= height;
	viewPort.MinZ		= 0.0f;
	viewPort.MaxZ		= 1.0f;

	(*ppDevice)->SetViewport(&viewPort);

	// return success
	return S_OK;
}

HRESULT Li_UTI_D3D9::fn_CleanD3D9(LPDIRECT3DDEVICE9 pDevice)
{
	// close and release the 3D device
	if (pDevice != NULL)
	{
		pDevice->Release();
		pDevice = NULL;
	}
	// return success
	return S_OK;
}

// Screen Capture
HRESULT Li_UTI_D3D9::fn_ScreenGrab(LPDIRECT3DDEVICE9 pDevice, char *fileName)
{
	HRESULT hr;

	// get display dimensions
	D3DDISPLAYMODE mode;
	if (FAILED(hr=pDevice->GetDisplayMode(0, &mode)))
		return hr;

	// create the directx surface to store the front buffer image
	LPDIRECT3DSURFACE9 surf;
	if (FAILED(hr=pDevice->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surf, NULL)))
		return hr;

	// rread front buffer
	if (FAILED(hr=pDevice->GetFrontBufferData(0, surf)))
	{
		surf->Release();
		return hr;
	}

	// write the entire surface to the requested file
	hr = D3DXSaveSurfaceToFile(fileName, D3DXIFF_BMP, surf, NULL, NULL);

	// release the surface
	surf->Release();

	// return status of save operation to caller
	return hr;
}