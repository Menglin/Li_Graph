#include "Li_UTI_UI.h"

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
