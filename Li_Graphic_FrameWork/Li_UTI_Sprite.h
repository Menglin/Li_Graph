#ifndef LI_UTI_DXSPRITE_H
#define LI_UTI_DXSPRITE_H

#include <d3dx9.h>


	  //////////////////////
	 // Sprite Interface //
	//////////////////////
class Li_Sprite_Interface
{
public:
	bool fn_InitSpriteInterface(IDirect3DDevice9* pDevice);
	bool fn_ReleaseSpriteInterface();

	HRESULT fn_spriteInterfaceBegin();
	HRESULT fn_spriteInterfaceEnd();
protected:
	LPD3DXSPRITE m_dxSpriteInterface;	// directx sprite interface object
};

	  /////////////////
	 // Base Sprite //
	/////////////////
class Li_Sprite
{
public:
	//void fn_setPosition(D3DXVECTOR3 pos);
	void fn_setTexture(IDirect3DDevice9* pDevice, LPCSTR pStrFile);
	void fn_releaseTexture();
	void fn_resetTexture(IDirect3DDevice9* pDevice, LPCSTR pStrFile);

	// draw the rect part of the sprite texture
	HRESULT fn_drawSprite(LPD3DXSPRITE dxSpriteInterface);
	HRESULT fn_drawSprite(LPD3DXSPRITE dxSpriteInterface, D3DXVECTOR3 translation,
						D3DXCOLOR blendCol = D3DXCOLOR(1, 1, 1, 1), RECT *rect = NULL);
	// draw a Frame by keep the resolution of margins
	void fn_drawAsBoarder(LPD3DXSPRITE dxSpriteInterface, RECT frame);

	void fn_setPosition(float x, float y)
	{
		m_pos.x = x;
		m_pos.y = y;
	}
	float fn_getx() {return m_pos.x;}
	float fn_gety() {return m_pos.y;}

	long fn_getHeight();
	long fn_getWidth();

protected:
	D3DXVECTOR2 m_pos;					// position of the sprite *this pos is the abs pos on the screen
	long m_Height;						// height of the texture
	long m_Width;						// width of the texture

	LPDIRECT3DTEXTURE9	m_dxTexture;	// texture
};

#endif