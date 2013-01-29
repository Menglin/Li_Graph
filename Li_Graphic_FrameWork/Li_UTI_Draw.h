#ifndef LI_UTI_DRAW_H
#define LI_UTI_DRAW_H

#include <d3dx9.h>

class Li_Draw
{
public:
	Li_Draw(){m_WndSize = D3DXVECTOR2(0,0);}
	virtual HRESULT fn_InitDraw(IDirect3DDevice9* pDeviceh, float w, float h);
	virtual HRESULT fn_DestroyDraw();

	// set Window size
	void fn_setWndSize(float w, float h);	// Although Dx already deal with the drawing viewport,, but it only on the render lever for the gpu time
											// set a wnd size here to constrain the drawing functions do not wast cpu time.
	virtual void fn_DrawLines(D3DXVECTOR2* points, DWORD vc, float width, D3DCOLOR col);
	virtual void fn_DrawLine(float x1, float y1, float x2, float y2, float lineWidth = 1.0, int r = 0, int g = 0, int b = 0, int a = 255);
	virtual void fn_DrawRectangle(float x, float y, float w, float h, float lineWidth = 1.0, int r = 0, int g = 0, int b = 0, int a = 255);
	virtual void fn_FillBox(float x, float y, float w, float h, int r = 0, int g = 0, int b = 0, int a = 255);
	virtual void fn_DrawCircle(float x, float y, float radius, float lineWidth = 1.0, int r = 0, int g = 0, int b = 0, int a = 255);
	virtual void fn_FillCircle(float x, float y, float radius, int inr = 0, int ing = 0, int inb = 0, int ina = 255, bool skirt = false, int outr = 0, int outg = 0, int outb = 0, int outa = 255);

protected:
	D3DXVECTOR2	m_WndSize;	// View port
	LPD3DXFONT	m_font;		// Font
	ID3DXLine	*m_pLine;	// Line
};

#endif