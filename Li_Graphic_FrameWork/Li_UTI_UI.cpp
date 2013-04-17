#include "Li_UTI_UI.h"

  ///////////////////
 // Button Sprite //
///////////////////
HRESULT Li_btnSprite::fn_drawButton(LPD3DXSPRITE dxSpriteInterface)
{
	// Build our matrix to rotate, scale and position our sprite
	D3DXMATRIX mat;
	RECT srcRect;

	srcRect.left	= 0;
	srcRect.right	= m_Width;

	switch (m_btnState)
	{
	case IDLE:
		{
			srcRect.top		= 0;
			srcRect.bottom	= m_Height / 3;
		} break;
	case PUSH:
		{
			srcRect.top		= m_Height / 3;
			srcRect.bottom	= m_Height / 3 * 2;
		} break;
	case PASS:
		{
			srcRect.top		= m_Height / 3 * 2;
			srcRect.bottom	= m_Height;
		} break;
	}

	D3DXVECTOR3 translation;
	translation.x = m_pos.x;
	translation.y = m_pos.y;
	translation.z = 0;

	// out, scaling centre, scaling rotation, scaling, rotation centre, rotation, translation
	D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &translation);
	dxSpriteInterface->SetTransform(&mat);

	return dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);
}

bool Li_btnSprite::fn_isMouseOn(D3DXVECTOR2 mousepos)
{
	if (mousepos.x > m_pos.x && mousepos.x < m_pos.x + m_Width &&
		mousepos.y > m_pos.y && mousepos.y < m_pos.y + m_Height / 3)
		return true;
	else
		return false;
}

  ////////////////////////
 // List Button Sprite //
////////////////////////

HRESULT Li_lstBtnSprite::fn_drawButton(LPD3DXSPRITE dxSpriteInterface)
{
	// Build our matrix to rotate, scale and position our sprite
	D3DXMATRIX mat;
	RECT srcRect;
	
	srcRect.left	= 0;
	srcRect.right	= m_Width;

	switch (m_direction)
	{
	case 0: // Down
		{
			// draw the selected button in the list
			srcRect.top		= m_Height / m_lstSize * m_curSelect;
			srcRect.bottom	= m_Height / m_lstSize * (m_curSelect + 1);
		} break;
	default: break;
	}

	D3DXVECTOR3 translation;
	translation.x = m_pos.x;
	translation.y = m_pos.y;
	translation.z = 0;

	// out, scaling centre, scaling rotation, scaling, rotation centre, rotation, translation
	D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &translation);
	dxSpriteInterface->SetTransform(&mat);

	return dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);
}

HRESULT Li_lstBtnSprite::fn_drawButtonList(LPD3DXSPRITE dxSpriteInterface)
{
	// Build our matrix to rotate, scale and position our sprite
	D3DXMATRIX mat;
	RECT srcRect;
	D3DXVECTOR3 translation;
	D3DCOLOR col;

	switch (m_direction)
	{
	case 0: // Down
		{
			// draw the list button separately
			for (int i = 0; i < m_lstSize; i++)
			{
				// draw the list at the different position
				srcRect.left	= 0;
				srcRect.right	= m_Width;
				srcRect.top		= m_Height / m_lstSize * i;
				srcRect.bottom	= m_Height / m_lstSize * (i + 1);

				translation.x	= m_pos.x;
				translation.y	= m_pos.y + m_Height / m_lstSize * (i + 1);
				translation.z = 0;

				if (i == m_curSelect)
					col = 0xFFFFFFFF;
				else
					col = 0x78787878;

				// out, scaling centre, scaling rotation, scaling, rotation centre, rotation, translation
				D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &translation);
				dxSpriteInterface->SetTransform(&mat);
				dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,col);
			}
		} break;
	default: break;
	}

	return S_OK;
}


bool Li_lstBtnSprite::fn_isMouseOn(D3DXVECTOR2 mousepos)
{
	long actHeight;
	
	switch (m_direction)
	{
	case 0: // down
		{
			// when the mouse pass the list, the list show, 
			// then the user should allowed to select from the list
			// so use the full m_Height / m_lstSize * (m_lstSize + 1)
			if (m_btnState == PASS)
				actHeight = m_Height / m_lstSize * (m_lstSize + 1);
			else
				actHeight = m_Height / m_lstSize;

			if (mousepos.x > m_pos.x && mousepos.x < m_pos.x + m_Width &&
				mousepos.y > m_pos.y && mousepos.y < m_pos.y + actHeight)
				return true;
			else
				return false;
		} break;
	}

	return false;
}

void Li_lstBtnSprite::fn_select(D3DXVECTOR2 mousepos)
{
	RECT srcRect;
	switch (m_direction)
	{
	case 0: // down
		{
			for (int i = 0; i < m_lstSize; i++)
			{
				srcRect.left	= (long)m_pos.x;
				srcRect.right	=(long) m_pos.x + m_Width;
				srcRect.top		= (long)m_pos.y + m_Height / m_lstSize * (i+1);
				srcRect.bottom	= (long)m_pos.y + m_Height / m_lstSize * (i+2);

				if (mousepos.x > srcRect.left && mousepos.x < srcRect.right &&
					mousepos.y > srcRect.top && mousepos.y < srcRect.bottom)
				{
					m_curSelect = i;
				}
			}
			// end for
		} break;
	}
}