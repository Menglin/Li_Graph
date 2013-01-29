#include "Li_UTI_Draw.h"
#include "../Li_ComnHeader/Li_MathConst.h"

HRESULT Li_Draw::fn_InitDraw(IDirect3DDevice9* pDevice, float w, float h)
{
	// Create a D3DX font object
	D3DXCreateFont(pDevice, 20, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_font);

	// create line object
	D3DXCreateLine(pDevice, &m_pLine);

	fn_setWndSize(w, h);

	return S_OK;
}

HRESULT Li_Draw::fn_DestroyDraw()
{
	// release font
	if (m_font != NULL)
	{
		m_font->Release();
		m_font = NULL;
	}

	// release line object
	if (m_pLine != NULL)
	{
		m_pLine->Release();
		m_pLine = NULL;
	}

	return S_OK;
}

void Li_Draw::fn_setWndSize(float w, float h)
{
	m_WndSize.x = w;
	m_WndSize.y = h;
}

void Li_Draw::fn_DrawLines(D3DXVECTOR2* points, DWORD vc, float width, D3DCOLOR col)
{
	m_pLine->SetWidth(width);
	m_pLine->SetPattern(0xffffffff);

	m_pLine->Begin();
	m_pLine->Draw(points, vc, col);
	m_pLine->End();
}

void Li_Draw::fn_DrawLine(float x1, float y1, float x2, float y2, float lineWidth, int r, int g, int b, int a)
{
	// draw circle if a part of the line is in the view port
	if (((x1 > 0 && y1 > 0) || (x2 > 0 && y2 > 0)) &&
		((x1 < m_WndSize.x && y1 < m_WndSize.y) || (x2 < m_WndSize.x && y2 < m_WndSize.y)))
	{
		D3DXVECTOR2 vLine[2];

		m_pLine->SetWidth(lineWidth);
		m_pLine->SetAntialias(false);
		m_pLine->SetGLLines(true);

		vLine[0].x = x1;
		vLine[0].y = y1;
		vLine[1].x = x2;
		vLine[1].y = y2;

		m_pLine->Begin();
		m_pLine->Draw(vLine, 2, D3DCOLOR_RGBA(r, g, b, a));
		m_pLine->End();
	}
}

void Li_Draw::fn_DrawRectangle(float x, float y, float w, float h, float lineWidth, int r, int g, int b, int a)
{
	// draw circle if a part of the rectangle is in the view port
	if ((x+w > 0 && y+h > 0) &&
		(x < m_WndSize.x && y < m_WndSize.y))
	{
		D3DXVECTOR2 vLine[5];

		m_pLine->SetWidth(lineWidth);
		m_pLine->SetAntialias(false);
		m_pLine->SetGLLines(true);

		vLine[0].x = x;
		vLine[0].y = y;
		vLine[1].x = x + w;
		vLine[1].y = y;
		vLine[2].x = x + w;
		vLine[2].y = y + h;
		vLine[3].x = x;
		vLine[3].y = y + h;
		vLine[4].x = x;
		vLine[4].y = y;

		m_pLine->Begin();
		m_pLine->Draw(vLine, 5, D3DCOLOR_RGBA(r, g, b, a));
		m_pLine->End();
	}
}

void Li_Draw::fn_FillBox(float x, float y, float w, float h, int r, int g, int b, int a)
{
	// draw circle if a part of the box is in the view port
	if ((x+w > 0 && y+h > 0) &&
		(x < m_WndSize.x && y-h < m_WndSize.y))
	{
		D3DXVECTOR2 vLine[2];

		m_pLine->SetWidth(w);
		m_pLine->SetAntialias(false);
		m_pLine->SetGLLines(true);

		vLine[0].x = x + w/2;
		vLine[0].y = y;
		vLine[1].x = x + w/2;
		vLine[1].y = y + h;

		m_pLine->Begin();
		m_pLine->Draw(vLine, 2, D3DCOLOR_RGBA(r, g, b, a));
		m_pLine->End();
	}
}

void Li_Draw::fn_DrawCircle(float x, float y, float radius, float lineWidth, int r, int g, int b, int a)
{
	// draw circle if a part of the circle is in the view port
	if ((x+radius > 0 && y+radius > 0) &&
		(x-radius < m_WndSize.x && y-radius < m_WndSize.y))
	{
		D3DXVECTOR2 vLine[2];

		m_pLine->SetWidth(lineWidth);
		m_pLine->SetAntialias(true);
		m_pLine->SetGLLines(true);

		m_pLine->Begin();

		float circleResolution = radius*2;
		if (circleResolution < 10)
			circleResolution = 10;
		else if (circleResolution > 100)
			circleResolution = 100;

		// 有时候末端连接有缝隙,所以分辨率加一,多画一点,避免缝隙出现
		for (int i = 0; i < circleResolution+1; i++)
		{
			vLine[0].x = x + radius *(float)cos(2 * PI * i / circleResolution);
			vLine[0].y = y + radius *(float)sin(2 * PI * i / circleResolution);
			vLine[1].x = x + radius *(float)cos(2 * PI * (i+1) / circleResolution);
			vLine[1].y = y + radius *(float)sin(2 * PI * (i+1) / circleResolution);
			m_pLine->Draw(vLine, 2, D3DCOLOR_RGBA(r, g, b, a));
		}

		m_pLine->End();
	}
}

void Li_Draw::fn_FillCircle(float x, float y, float radius, int inr, int ing, int inb, int ina, bool skirt, int outr, int outg, int outb, int outa)
{
	// draw circle if a part of the circle is in the view port
	if ((x+radius > 0 && y+radius > 0) &&
		(x-radius < m_WndSize.x && y-radius < m_WndSize.y))
	{
		// draw the out skirt
		if (skirt)
			fn_DrawCircle(x, y, radius, 1, outr, outg, outb, outa);
		else
			fn_DrawCircle(x, y, radius, 1, inr, ing, inb, ina);

		D3DXVECTOR2 vLine[2];

		m_pLine->SetWidth(3.0);
		m_pLine->SetAntialias(true);
		m_pLine->SetGLLines(true);

		m_pLine->Begin();

		/*
		for (int j = 0; j < radius; j++)
		{
			int circleResolution = j*2;
			if (circleResolution < 10)
				circleResolution = 10;
			else if (circleResolution > 100)
				circleResolution = 100;

			// 有时候末端连接有缝隙,所以分辨率加一,多画一点,避免缝隙出现
			for (int i = 0; i < circleResolution+1; i++)
			{
				vLine[0].x = x + j *(float)cos(2 * PI * i / circleResolution);
				vLine[0].y = y + j *(float)sin(2 * PI * i / circleResolution);
				vLine[1].x = x + j *(float)cos(2 * PI * (i+1) / circleResolution);
				vLine[1].y = y + j *(float)sin(2 * PI * (i+1) / circleResolution);
				m_pLine->Draw(vLine, 2, D3DCOLOR_RGBA(r, g, b, a));
			}
		}
		*/

		float circleResolution = radius*2;
		if (circleResolution < 10)
			circleResolution = 10;
		else if (circleResolution > 100)
			circleResolution = 100;

		// 有时候末端连接有缝隙,所以分辨率加一,多画一点,避免缝隙出现
		for (int i = 0; i < circleResolution+1; i++)
		{
			for (int j = 0; j < radius; j++)
			{
				vLine[0].x = x + j *(float)cos(2 * PI * i / circleResolution);
				vLine[0].y = y + j *(float)sin(2 * PI * i / circleResolution);
				vLine[1].x = x + j *(float)cos(2 * PI * (i+1) / circleResolution);
				vLine[1].y = y + j *(float)sin(2 * PI * (i+1) / circleResolution);
				m_pLine->Draw(vLine, 2, D3DCOLOR_RGBA(inr, ing, inb, ina));
			}
		}

		m_pLine->End();
	}
}