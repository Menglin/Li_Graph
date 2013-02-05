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
protected:
	BTN_STATE m_btnState;
};

#endif