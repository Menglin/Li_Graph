#include "Li_UTI_Sprite.h"

	  //////////////////////
	 // Sprite Interface //
	//////////////////////

bool Li_Sprite_Interface::fn_InitSpriteInterface(IDirect3DDevice9* pDevice)
{
	// Create direct sprite object
	if ( !SUCCEEDED( D3DXCreateSprite(pDevice, &m_dxSpriteInterface) ) )
	{
		MessageBox(NULL, "D3DXCreateSprite() - FAILED!", "Failure!", MB_OK);
		// return false
		return false;
	}

	return true;
}

bool Li_Sprite_Interface::fn_ReleaseSpriteInterface()
{
	m_dxSpriteInterface->Release();

	return true;
}

HRESULT Li_Sprite_Interface::fn_spriteInterfaceBegin()
{
	// begin with alpha enabled
	return m_dxSpriteInterface->Begin(D3DXSPRITE_ALPHABLEND);
}

HRESULT Li_Sprite_Interface::fn_spriteInterfaceEnd()
{
	return m_dxSpriteInterface->End();
}


	  /////////////////
	 // Base Sprite //
	/////////////////

HRESULT Li_Sprite::fn_drawSprite(LPD3DXSPRITE dxSpriteInterface, D3DXVECTOR3 translation, D3DXCOLOR blendCol, RECT *pSrcRect)
{
	// Build our matrix to rotate, scale and position our sprite
	D3DXMATRIX mat;

	/*
	D3DXVECTOR3 pScalingCenter=D3DXVECTOR3(m_Width/2.0f, m_Height/2.0f, 0.0f);
	D3DXQUATERNION  pScalingRotation = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 pScaling(1.0f, 1.0f, 1.0f);
	D3DXVECTOR3 pRotationCenter = D3DXVECTOR3(m_Width/2.0f, m_Height/2.0f, 0.0f);
	D3DXQUATERNION  pRotation = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 pTranslation = translation;

	// out, scaling centre, scaling rotation, scaling, rotation centre, rotation, translation
	D3DXMatrixTransformation(&mat, &pScalingCenter, &pScalingRotation, &pScaling, &pRotationCenter, &pRotation, &pTranslation);
	*/

	// out, scaling centre, scaling rotation, scaling, rotation centre, rotation, translation
	D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &translation);

	// Tell the sprite about the matrix
	dxSpriteInterface->SetTransform(&mat);
	return dxSpriteInterface->Draw(m_dxTexture,pSrcRect,NULL,NULL,blendCol);

	// draw without advanced feature only need this line
	//return m_dxSpriteInterface->Draw(m_dxTexture,rect,NULL,&m_Pos,0xFFFFFFFF);
}

// draw a Frame by keep the resolution of margins
// 从中间把图劈开,分为四角,按照图案填补四边和中心
void Li_Sprite::fn_drawAsBoarder(LPD3DXSPRITE dxSpriteInterface, RECT frame)
{
	D3DXMATRIX mat;
	RECT srcRect;
	long xCenter = m_Width/2;
	long yCenter = m_Height/2;

	D3DXVECTOR3 tmpPos(0,0,0);
	D3DXVECTOR3 tmpScale;

	// Step 1. Draw Coners
	// top-left coner
	srcRect.left	= 0;
	srcRect.top		= 0;
	srcRect.right	= xCenter;
	srcRect.bottom	= yCenter;
	tmpPos.x		= (float)frame.left;
	tmpPos.y		= (float)frame.top;
	tmpPos.z		= 0.0f;
	D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &tmpPos);
	dxSpriteInterface->SetTransform(&mat);
	dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL, NULL,0xFFFFFFFF);

	// top-right coner
	srcRect.left	= xCenter;
	srcRect.top		= 0;
	srcRect.right	= m_Width;
	srcRect.bottom	= yCenter;
	tmpPos.x		= (float)frame.right - (srcRect.right - srcRect.left);
	tmpPos.y		= (float)frame.top;
	tmpPos.z		= 0.0f;
	D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &tmpPos);
	dxSpriteInterface->SetTransform(&mat);
	dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);

	// bottom-right coner
	srcRect.left	= xCenter;
	srcRect.top		= yCenter;
	srcRect.right	= m_Width;
	srcRect.bottom	= m_Height;
	tmpPos.x		= (float)frame.right - (srcRect.right - srcRect.left);
	tmpPos.y		= (float)frame.bottom - (srcRect.bottom - srcRect.top);
	tmpPos.z		= 0.0f;
	D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &tmpPos);
	dxSpriteInterface->SetTransform(&mat);
	dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);

	// bottom-left coner
	srcRect.left	= 0;
	srcRect.top		= yCenter;
	srcRect.right	= xCenter;
	srcRect.bottom	= m_Height;
	tmpPos.x		= (float)frame.left;
	tmpPos.y		= (float)frame.bottom - (srcRect.bottom - srcRect.top);
	tmpPos.z		= 0.0f;
	D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &tmpPos);
	dxSpriteInterface->SetTransform(&mat);
	dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);

	// Step 2. Draw Margins
	// left margin
	srcRect.left	= 0;
	srcRect.top		= yCenter;
	srcRect.right	= xCenter;
	srcRect.bottom	= yCenter+1;
	tmpPos.x		= (float)frame.left + srcRect.left;
	tmpPos.y		= (float)frame.top + srcRect.top;
	tmpPos.z		= 0.0f;
	tmpScale = D3DXVECTOR3(1.0f, float(frame.bottom - frame.top - m_Height), 1.0f);
	D3DXMatrixTransformation(&mat, NULL, NULL, &tmpScale, NULL, NULL, &tmpPos);
	dxSpriteInterface->SetTransform(&mat);
	dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);
	/*
	for (int i = 0; i < (frame.bottom - frame.top - m_Height); i++)
	{
		D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &tmpPos);
		dxSpriteInterface->SetTransform(&mat);
		dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);
		tmpPos.y++;
	}
	*/

	// top margin
	srcRect.left	= xCenter;
	srcRect.top		= 0;
	srcRect.right	= xCenter+1;
	srcRect.bottom	= yCenter;
	tmpPos.x		= (float)frame.left + srcRect.left;
	tmpPos.y		= (float)frame.top + srcRect.top;
	tmpPos.z		= 0.0f;
	tmpScale = D3DXVECTOR3(float(frame.right - frame.left - m_Width), 1.0f, 1.0f);
	D3DXMatrixTransformation(&mat, NULL, NULL, &tmpScale, NULL, NULL, &tmpPos);
	dxSpriteInterface->SetTransform(&mat);
	dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);
	/*
	for (int i = 0; i < (frame.right - frame.left - m_Width); i++)
	{
		D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &tmpPos);
		dxSpriteInterface->SetTransform(&mat);
		dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);
		tmpPos.x++;
	}
	*/

	// right margin
	srcRect.left	= xCenter;
	srcRect.top		= yCenter;
	srcRect.right	= m_Width;
	srcRect.bottom	= yCenter+1;
	tmpPos.x		= (float)frame.right - (srcRect.right - srcRect.left);
	tmpPos.y		= (float)frame.top + srcRect.top;
	tmpPos.z		= 0.0f;
	tmpScale = D3DXVECTOR3(1.0f, float(frame.bottom - frame.top - m_Height), 1.0f);
	D3DXMatrixTransformation(&mat, NULL, NULL, &tmpScale, NULL, NULL, &tmpPos);
	dxSpriteInterface->SetTransform(&mat);
	dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);
	/*
	for (int i = 0; i < (frame.bottom - frame.top - m_Height); i++)
	{
		D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &tmpPos);
		dxSpriteInterface->SetTransform(&mat);
		dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);
		tmpPos.y++;
	}
	*/

	// bottom margin
	srcRect.left	= xCenter;
	srcRect.top		= yCenter;
	srcRect.right	= xCenter+1;
	srcRect.bottom	= m_Height;
	tmpPos.x		= (float)frame.left + srcRect.left;
	tmpPos.y		= (float)frame.bottom - (srcRect.bottom - srcRect.top);
	tmpPos.z		= 0.0f;
	tmpScale = D3DXVECTOR3(float(frame.right - frame.left - m_Width), 1.0f, 1.0f);
	D3DXMatrixTransformation(&mat, NULL, NULL, &tmpScale, NULL, NULL, &tmpPos);
	dxSpriteInterface->SetTransform(&mat);
	dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);
	/*
	for (int i = 0; i < (frame.right - frame.left - m_Width); i++)
	{
		D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &tmpPos);
		dxSpriteInterface->SetTransform(&mat);
		dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);
		tmpPos.x++;
	}
	*/

	// fill the inside
	srcRect.left	= xCenter;
	srcRect.top		= yCenter;
	srcRect.right	= xCenter+1;
	srcRect.bottom	= yCenter+1;
	tmpPos.x		= (float)frame.left + srcRect.left;
	tmpPos.y		= (float)frame.top + srcRect.top;
	tmpPos.z		= 0.0f;
	tmpScale = D3DXVECTOR3(float(frame.right - frame.left - m_Width), float(frame.bottom - frame.top - m_Height), 1.0f);
	D3DXMatrixTransformation(&mat, NULL, NULL, &tmpScale, NULL, NULL, &tmpPos);
	dxSpriteInterface->SetTransform(&mat);
	dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);
	/* // This approach is wast cpu time
	for (int i = 0; i < (frame.right - frame.left - m_Width); i++)
	{
		for (int j = 0; j < (frame.bottom - frame.top - m_Height); j++)
		{
			D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &tmpPos);
			dxSpriteInterface->SetTransform(&mat);
			dxSpriteInterface->Draw(m_dxTexture,&srcRect,NULL,NULL,0xFFFFFFFF);
			tmpPos.y++;
		}
		tmpPos.x++;
		tmpPos.y = (float)frame.top + tmpRect.top;
	}
	*/

	// 把矩阵转换重置到输入的地方,外界可以对其直接使用text.draw
	// 可输入的地方就是四周边框之内的
	tmpPos.x		= (float)frame.left + xCenter;
	tmpPos.y		= (float)frame.top + yCenter;
	tmpPos.z		= 0.0f;
	D3DXMatrixTransformation(&mat, NULL, NULL, NULL, NULL, NULL, &tmpPos);
	dxSpriteInterface->SetTransform(&mat);
	// end for
}

void Li_Sprite::fn_setTexture(IDirect3DDevice9* pDevice, LPCSTR pStrFile)
{
	// 使用D3DXCreateTextureFromFile 会默认材质大小优化为2的整数幂，非正规图像会被拉伸，因此使用Ex方法，D3DX_FROM_FILE从文件读取长宽
	D3DXCreateTextureFromFileEx(pDevice,				// the device pointer
							pStrFile,					// the file name
							D3DX_FROM_FILE,				// default width
							D3DX_FROM_FILE,				// default height
							D3DX_DEFAULT,				// no mip mapping
							NULL,						// regular usage
							D3DFMT_A8R8G8B8,			// 32-bit pixels with alpha
							D3DPOOL_MANAGED,			// typical memory handling
							D3DX_DEFAULT,				// no filtering
							D3DX_DEFAULT,				// no mip filtering
							D3DCOLOR_XRGB(255, 0, 255),	// the hot-pink color key
							NULL,						// no image info struct
							NULL,						// not using 256 colors
							&m_dxTexture);				// load to texture_1

	// Get the dimensions of our txture
	D3DSURFACE_DESC	desc;
	m_dxTexture->GetLevelDesc(0,&desc);

	m_Height	= desc.Width;
	m_Width		= desc.Height;
}

void Li_Sprite::fn_releaseTexture()
{
	m_dxTexture->Release();
}

void Li_Sprite::fn_resetTexture(IDirect3DDevice9* pDevice, LPCSTR pStrFile)
{
	m_dxTexture->Release();
	fn_setTexture(pDevice, pStrFile);
}

long Li_Sprite::fn_getHeight()
{
	return m_Height;
}

long Li_Sprite::fn_getWidth()
{
	return m_Width;
}