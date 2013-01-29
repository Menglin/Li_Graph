#include "Li_GraphBrowser.h"
#include "../Li_ComnHeader/Li_ComnFunc.h"

  /*-----------------
 / Graph Functions /
-----------------*/

void Li_GraphBrowser::fn_DrawGraph()
{
	int num = m_Nodes.size();
	float nodeR			= 0.0f;
	D3DXVECTOR2 tmpVec	= D3DXVECTOR2(0,0);
	D3DXVECTOR2 tmpVec2	= D3DXVECTOR2(0,0);
	int drawTip			= -1;

	// draw nodes and edges
	for (int i = 0; i < num; i++)
	{
		// clip nodes
		tmpVec = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_PosX, m_Nodes[i]->m_PosY));

		// get texture radius
		nodeR = m_nodeSprite.fn_getWidth() / 2.0f;

		// draw the node which is grabed red
		if (m_Nodes[i]->m_isSelected)
		{
			m_nodeSprite.fn_drawSprite(m_dxSpriteInterface,
						D3DXVECTOR3(tmpVec.x - m_nodeSprite.fn_getWidth()/2, tmpVec.y - m_nodeSprite.fn_getHeight()/2, 0),
						D3DXCOLOR(1, 0.25f, 0.25f, 1));

			D3DXVECTOR2 newPos = m_CamMan->fn_getAbsPos(s_MousePosAbs);

			m_Nodes[i]->m_PosX = newPos.x;
			m_Nodes[i]->m_PosY = newPos.y;

			// set draw Tip
			drawTip = i;
		}
		// draw nodes, if mouse on it ,draw it blue, else, gray
		else if ((s_MousePosAbs.x - tmpVec.x)*(s_MousePosAbs.x - tmpVec.x) +
			(s_MousePosAbs.y - tmpVec.y)*(s_MousePosAbs.y - tmpVec.y) <= nodeR*nodeR
			&& drawTip < i)
		{
			m_nodeSprite.fn_drawSprite(m_dxSpriteInterface,
									D3DXVECTOR3(tmpVec.x - m_nodeSprite.fn_getWidth()/2, tmpVec.y - m_nodeSprite.fn_getHeight()/2, 0),
									D3DXCOLOR(0.25f, 0.75f, 1, 1));

			// set draw Tip
			drawTip = i;
		}
		else
		{
			m_nodeSprite.fn_drawSprite(m_dxSpriteInterface,
									D3DXVECTOR3(tmpVec.x - m_nodeSprite.fn_getWidth()/2, tmpVec.y - m_nodeSprite.fn_getHeight()/2, 0));
		}
		// end if-else

		// draw connections
		for (unsigned int j = 0; j < m_Nodes[i]->m_Conn.size(); j++)
		{
			tmpVec2 = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_Conn[j]->m_PosX, m_Nodes[i]->m_Conn[j]->m_PosY));

			if (m_Nodes[i]->m_isSelected)
			{
				fn_DrawLine(tmpVec.x, tmpVec.y, tmpVec2.x, tmpVec2.y, 2, 0, 0, 255);
			}
			else if (m_Nodes[i]->m_Conn[j]->m_ID > m_Nodes[i]->m_ID)
			{
				// if the edge is linked to the selected node, print it green, else red
				if ((s_MousePosAbs.x - tmpVec.x)*(s_MousePosAbs.x - tmpVec.x) +
							(s_MousePosAbs.y - tmpVec.y)*(s_MousePosAbs.y - tmpVec.y) <= nodeR*nodeR ||
					(s_MousePosAbs.x - tmpVec2.x)*(s_MousePosAbs.x - tmpVec2.x) +
							(s_MousePosAbs.y - tmpVec2.y)*(s_MousePosAbs.y - tmpVec2.y) <= nodeR*nodeR)
				{
					fn_DrawLine(tmpVec.x, tmpVec.y, tmpVec2.x, tmpVec2.y, 2, 0,255,0);
				}
				else
					fn_DrawLine(tmpVec.x, tmpVec.y, tmpVec2.x, tmpVec2.y, 2, 255);
				// end if
			}
			// end if
		}
		// end for j
	}
	// end for i


	// draw graph attributes ////
	// Plot Node Info
	RECT rct;
	rct.left	= m_WinWidth - 400;
	rct.right	= m_WinWidth - 40;
	rct.top		= 20;
	rct.bottom	= 160;

	// draw m_textBoxSprite
	m_textBoxSprite.fn_drawAsBoarder(m_dxSpriteInterface, rct);

	char tmpStr[256];
	strcpy_s(tmpStr, "The Graph Attributes: ");
	strcat_s(tmpStr, "\nThe Graph Size: ");
	strcat_s(tmpStr, ParseStr(m_GraphSize));
	strcat_s(tmpStr, "\nAverage Degree: ");
	strcat_s(tmpStr, ParseStr(m_AvgDegree, 6));
	strcat_s(tmpStr, "\nTotal Clustering Coefficient: ");
	strcat_s(tmpStr, ParseStr(m_TotalCC, 6));
	strcat_s(tmpStr, "\nAverage Clustering Coefficient: ");
	strcat_s(tmpStr, ParseStr(m_AvgCC, 6));
	m_font->DrawText(m_dxSpriteInterface,  tmpStr, -1, NULL, DT_LEFT, D3DCOLOR(0xFF000000));

	// user manu
	rct.left = m_WinWidth / 2 - 310;
	rct.right = m_WinWidth / 2 + 310;
	rct.top = m_WinHeight - 120;
	rct.bottom = m_WinHeight - 50;

	// draw m_textBoxSprite
	m_textBoxSprite.fn_drawAsBoarder(m_dxSpriteInterface, rct);

	strcpy_s(tmpStr, "Press 'ESC' to quit, press 'R' to reset position\n");
	strcat_s(tmpStr, "Left Btn: move the Node. Right Btn: move the scene. Mouse wheel: Scale.");
	m_font->DrawText(m_dxSpriteInterface,  tmpStr, -1, NULL, DT_CENTER, D3DCOLOR(0xFFFF0F0F));


	// draw Tip ////
	if (drawTip != -1)
	{
		// clip nodes
		tmpVec = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[drawTip]->m_PosX, m_Nodes[drawTip]->m_PosY));

		// Plot Node Info
		RECT rct;
		rct.left	= (long)tmpVec.x + 20;
		rct.right	= rct.left + 150;
		rct.top		= (long)tmpVec.y + 10;
		rct.bottom	= rct.top + 125;

		// draw m_textBoxSprite
		m_textBoxSprite.fn_drawAsBoarder(m_dxSpriteInterface, rct);

		char tmpStr[128];
		strcpy_s(tmpStr, "Node ID: ");
		strcat_s(tmpStr, ParseStr(m_Nodes[drawTip]->m_ID));
		strcat_s(tmpStr, "\nLocDegree : ");
		strcat_s(tmpStr, ParseStr(m_Nodes[drawTip]->m_LocDegree));
		strcat_s(tmpStr, "\nLocCC : ");
		strcat_s(tmpStr, ParseStr(m_Nodes[drawTip]->m_LocCC, 4));
		strcat_s(tmpStr, "\nStrategy : ");
		strcat_s(tmpStr, ParseStr(m_Nodes[drawTip]->m_Agent->m_Strategy));
		strcat_s(tmpStr, "\nPayoff : ");
		strcat_s(tmpStr, ParseStr(m_Nodes[drawTip]->m_Agent->m_TotalPayoff, 4));
		m_font->DrawText(m_dxSpriteInterface,  tmpStr, -1, NULL, DT_LEFT, D3DCOLOR(0xFF000000));
	}
	// end if
}


  /*---------------------
 / FrameWork Functions /
---------------------*/

HRESULT Li_GraphBrowser::fn_ctrl_initD3D()
{
	HRESULT r = Li_FW::fn_ctrl_initD3D();

	//// initial new d3d obj here ////
	// set Node texture
	m_nodeSprite.fn_setTexture(m_d3dDev, "../res/sdk_handle.png");
	// set textbox texture
	m_textBoxSprite.fn_setTexture(m_d3dDev, "../res/sdk_mini_text_box.png");
	//// initial new d3d obj here ////

	return r;
}

HRESULT Li_GraphBrowser::fn_ctrl_releaseD3D()
{
	//// release new d3d obj here ////
	// release Node texture
	m_nodeSprite.fn_releaseTexture();
	// release textbox texture
	m_textBoxSprite.fn_releaseTexture();
	//// release new d3d obj here ////

	return Li_FW::fn_ctrl_releaseD3D();
}

void Li_GraphBrowser::fn_ctrl_mainLogic()
{
	//// Mouse Left Button ////
	static bool alreadySelectedANode = false;
	// mouse left button down
	if (s_isLBtnDown)
	{
		if (!alreadySelectedANode)
		{
			float nodeR = m_nodeSprite.fn_getWidth() / 2.0f;

			// loop from the back to front, make sure always select the one on top
			for (int i = (int)m_Nodes.size() - 1; i >= 0; i--)
			{
				D3DXVECTOR2 tmpVec = D3DXVECTOR2(0,0);
				// clip nodes
				tmpVec = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_PosX, m_Nodes[i]->m_PosY));

				if (((s_MousePosAbs.x - tmpVec.x)*(s_MousePosAbs.x - tmpVec.x) +
					(s_MousePosAbs.y - tmpVec.y)*(s_MousePosAbs.y - tmpVec.y) <= nodeR*nodeR) &&
					(!alreadySelectedANode))
				{
					m_Nodes[i]->m_isSelected = true;
					alreadySelectedANode = true;

					// only one can been selected at a time, deal with overlap nodes
					break;
				}
			}
			// end for
		}
		// end if

		// update frame when the mous button down, so we could move the node in the render loop
		// save from using another loop
		fn_UpdateFrame();
	}
	// mouse left up
	else
	{
		for (unsigned int i = 0; i < m_Nodes.size(); i++)
		{
			// release all
			m_Nodes[i]->m_isSelected = false;
		}
		alreadySelectedANode = false;
		// end for
	}
	// end if
	//// end Mouse Left Button ////

	//// Mouse Right Button ////
	// right btn down
	if (s_isRBtnDown)
	{
	}
	//// end Mouse Right Button ////

	//// Keyboard inputs ////
	// input R to reinit the graph
	if(m_DxInput->m_KeyState[DIK_R] & 0x80)
	{
		fn_InitNodePos();
		fn_UpdateFrame();
	}

	// input ESC to quit
	if(m_DxInput->m_KeyState[DIK_ESCAPE] & 0x80)
		PostQuitMessage(0);
	//// Keyboard inputs ////
}

void Li_GraphBrowser::fn_ctrl_d3dRender()
{
	fn_DrawGraph();
}