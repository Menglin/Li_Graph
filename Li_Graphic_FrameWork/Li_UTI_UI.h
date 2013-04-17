#ifndef LI_UTI_UI_H
#define LI_UTI_UI_H

#include "Li_UTI_Sprite.h"

static enum BTN_STATE {IDLE, PUSH, PASS};

// Button
class Li_btnSprite : public Li_Sprite
{
public:
	Li_btnSprite()
	{
		m_btnState = IDLE;
	}

	void fn_setbtnState(BTN_STATE s)
	{
		m_btnState = s;
	}

	BTN_STATE fn_getState()
	{
		return m_btnState;
	}

	HRESULT fn_drawButton(LPD3DXSPRITE dxSpriteInterface);
	bool fn_isMouseOn(D3DXVECTOR2 mousepos);
protected:
	BTN_STATE m_btnState;
};

// List Button
class Li_lstBtnSprite : public Li_Sprite
{
public:
	Li_lstBtnSprite()
	{
		m_btnState = IDLE;
		m_lstSize = 3;
		m_curSelect = 0;
		m_direction = 0;
	}
	Li_lstBtnSprite(int lstSize, int dir = 0)
	{
		m_btnState = IDLE;
		m_lstSize = lstSize;
		m_curSelect = 0;
		m_direction = dir;
	}

	void fn_setbtnState(BTN_STATE s)
	{
		m_btnState = s;
	}

	BTN_STATE fn_getState()
	{
		return m_btnState;
	}

	int fn_getCurrentSelected()
	{
		return m_curSelect;
	}

	void fn_setCurSelecte(int i)
	{
		m_curSelect = i;
	}

	HRESULT fn_drawButton(LPD3DXSPRITE dxSpriteInterface);
	HRESULT fn_drawButtonList(LPD3DXSPRITE dxSpriteInterface);
	bool fn_isMouseOn(D3DXVECTOR2 mousepos);
	void fn_select(D3DXVECTOR2 mousepos);

protected:
	BTN_STATE m_btnState;
	int m_lstSize;
	int m_curSelect;
	int m_direction;// direction, 0: down, 1, right, 2, up, 3, left
};

#endif