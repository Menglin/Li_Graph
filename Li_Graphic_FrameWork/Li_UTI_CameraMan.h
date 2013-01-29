#ifndef LI_CAMERAMAN_H
#define LI_CAMERAMAN_H

#include <d3dx9math.h>

class Li_CameraMan
{
public:
	Li_CameraMan()
	{
		m_CameraPos		= D3DXVECTOR2(0,0);
		m_CameraScale	= 1.0;
		m_scaleSize		= 1.5;
	}

	// get a point's Relative position from the absolute position
	D3DXVECTOR2 fn_getRelPos(D3DXVECTOR2 absPos)
	{
		D3DXVECTOR2 refPos;

		refPos.x = (absPos.x - m_CameraPos.x) * m_CameraScale;
		refPos.y = (absPos.y - m_CameraPos.y) * m_CameraScale;

		return refPos;
	}

	// get a point's absolute position from the Relative position
	D3DXVECTOR2 fn_getAbsPos(D3DXVECTOR2 relPos)
	{
		D3DXVECTOR2 absPos;

		absPos.x = relPos.x / m_CameraScale + m_CameraPos.x;
		absPos.y = relPos.y / m_CameraScale + m_CameraPos.y;

		return absPos;
	}

	// move camera by the offset
	void fn_MoveCamera(D3DXVECTOR2 offset)
	{
		m_CameraPos.x += (offset.x / m_CameraScale);
		m_CameraPos.y += (offset.y / m_CameraScale);
	}

	// move camera to position
	void fn_MoveCameraTo(D3DXVECTOR2 pos)
	{
		m_CameraPos.x = pos.x / m_CameraScale;
		m_CameraPos.y = pos.y / m_CameraScale;
	}

	void fn_scaleUp(D3DXVECTOR2 mousePos)
	{
		// �Ŵ�ʱ��, �ȷŴ�, Ϊ�˱��������ָ������,ֻҪ���������귽���ƶ� ���λ�� * (�Ŵ���-1) ��,����
		m_CameraScale *= m_scaleSize;
		fn_MoveCamera(D3DXVECTOR2(mousePos.x * (m_scaleSize - 1), mousePos.y * (m_scaleSize - 1)));
	}

	void fn_scaleDown(D3DXVECTOR2 mousePos)
	{
		// ��Сʱ��, Ϊ�˱��������ָ������,ֻҪ���������귴�����ƶ� ���λ�� * (�Ŵ���-1) ��, ����С����
		fn_MoveCamera(D3DXVECTOR2(- mousePos.x * (m_scaleSize - 1), -mousePos.y * (m_scaleSize - 1)));
		m_CameraScale /= m_scaleSize;
	}

protected:
	// Camera position on the map
	D3DXVECTOR2 m_CameraPos;	// currently camera position

	float m_CameraScale;		// currently camera Scale
	float m_scaleSize;			// camera scale size for each time scroll the mouse wheel
};

#endif