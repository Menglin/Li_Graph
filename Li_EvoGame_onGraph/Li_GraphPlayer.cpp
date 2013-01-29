#include "Li_GraphPlayer.h"
#include "../Li_ComnHeader/Li_ComnFunc.h"

  /*-----------------
 / Graph Functions /
-----------------*/

void Li_GraphPlayer::fn_DrawGraph()
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

		// node is grabed
		if (m_Nodes[i]->m_isSelected)
		{
			D3DXVECTOR2 newPos = m_CamMan->fn_getAbsPos(s_MousePosAbs);

			m_Nodes[i]->m_PosX = newPos.x;
			m_Nodes[i]->m_PosY = newPos.y;

			// set draw Tip
			drawTip = i;
		}
		// if mouse on it
		else if ((s_MousePosAbs.x - tmpVec.x)*(s_MousePosAbs.x - tmpVec.x) +
			(s_MousePosAbs.y - tmpVec.y)*(s_MousePosAbs.y - tmpVec.y) <= nodeR*nodeR
			&& drawTip < i)
		{
			// set draw Tip
			drawTip = i;
		}

		// draw nodes depends on it's strategy
		if (m_Nodes[i]->m_Agent->m_Strategy == 0) // C, Blue
			m_nodeSprite.fn_drawSprite(m_dxSpriteInterface,
						D3DXVECTOR3(tmpVec.x - m_nodeSprite.fn_getWidth()/2, tmpVec.y - m_nodeSprite.fn_getHeight()/2, 0),
						D3DXCOLOR(0.25f, 0.5f, 1.0f, 1));
		else if (m_Nodes[i]->m_Agent->m_Strategy == 1) // D, Red
			m_nodeSprite.fn_drawSprite(m_dxSpriteInterface,
						D3DXVECTOR3(tmpVec.x - m_nodeSprite.fn_getWidth()/2, tmpVec.y - m_nodeSprite.fn_getHeight()/2, 0),
						D3DXCOLOR(1, 0.25f, 0.25f, 1));
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

	// draw Tip
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

	// draw graph attributes
	// Plot Node Info
	RECT rct;
	rct.left	= m_WinWidth - 400;
	rct.right	= m_WinWidth - 40;
	rct.top		= 20;
	rct.bottom	= 200;

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
	strcat_s(tmpStr, "\nGeneration Number: ");
	strcat_s(tmpStr, ParseStr(m_GenNum));
	strcat_s(tmpStr, "\nCooperation Rate: ");
	strcat_s(tmpStr, ParseStr(m_CR, 6));
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
}



  /*---------------------
 / FrameWork Functions /
---------------------*/

HRESULT Li_GraphPlayer::fn_ctrl_initD3D()
{
	HRESULT r = Li_GraphBrowser::fn_ctrl_initD3D();

	//// initial new d3d obj here ////

	//// initial new d3d obj here ////

	return r;
}

HRESULT Li_GraphPlayer::fn_ctrl_releaseD3D()
{
	//// release new d3d obj here ////

	//// release new d3d obj here ////

	return Li_GraphBrowser::fn_ctrl_releaseD3D();
}

void Li_GraphPlayer::fn_ctrl_mainLogic()
{
	Li_GraphBrowser::fn_ctrl_mainLogic();


	fn_goEpic();
}

void Li_GraphPlayer::fn_ctrl_d3dRender()
{
	fn_DrawGraph();
}

void Li_GraphPlayer::fn_goEpic()
{
	time_t curtime = time(NULL);
	if (curtime - m_LastUpdateTime > 0.2)
	{
		fn_updateGen();
		fn_calPayoff();
		fn_calCR();
		fn_UpdateFrame();
		m_LastUpdateTime = curtime;
	}

	//// Keyboard inputs ////
	// input R to reinit the graph
	if(m_DxInput->m_KeyState[DIK_R] & 0x80)
	{
		fn_initStrategy();
	}
}

void Li_GraphPlayer::fn_calPayoff()
{
	// 1. empty all payoff
	int tnum = m_Nodes.size();
	for (int i = 0; i < tnum; i++)
	{
		m_Nodes[i]->m_Agent->m_TotalPayoff = 0.0;
	}

	// 2. calculate payoff
	for (int i = 0; i < tnum; i++)
	{
		// self interaction
		if (isSelfInteract)
		{
			if (m_Nodes[i]->m_Agent->m_TotalPayoff == C)
				m_Nodes[i]->m_Agent->m_TotalPayoff += R;
			else
				m_Nodes[i]->m_Agent->m_TotalPayoff += P;
		}

		int tsize = m_Nodes[i]->m_Conn.size();
		for (int j = 0; j < tsize; j++)
		{
			fn_Play(m_Nodes[i], m_Nodes[i]->m_Conn[j], false);
		}
	}
	// end for
}

// calculate cooperation rate
void Li_GraphPlayer::fn_calCR()
{
	int CN = 0;
	int tnum = m_Nodes.size();
	for (int i = 0; i < tnum; i++)
	{
		if (m_Nodes[i]->m_Agent->m_Strategy == 0)
			CN++;
	}

	m_CR = CN * 1.0 / tnum;
}

void Li_GraphPlayer::fn_updateGen()
{
	// 1. adopt new strategy
	// in this step, the new strategy is saved in mUpdateStrategy
	int tnum = m_Nodes.size();
	for (int i = 0; i < tnum; i++)
	{
		fn_Evolve(m_Nodes[i]);
		// end if
	}
	// end for

	// 2. update strategy
	for (int i = 0; i < tnum; i++)
	{
		// ovserve strategy is decide by the iterate strategy
		STRATEGY ts = m_Nodes[i]->m_Agent->m_Strategy;
		m_Nodes[i]->m_Agent->m_Strategy = m_Nodes[i]->m_Agent->m_UpdateStrategy;
		m_Nodes[i]->m_Agent->m_UpdateStrategy = ts;

		// these done by the evolve function
		m_Nodes[i]->m_Agent->m_UpdateStrategy = m_Nodes[i]->m_Agent->m_Strategy;
	}
	// for the iterate game

	m_GenNum++;
}

void Li_GraphPlayer::fn_initStrategy()
{
	int tnum = m_Nodes.size();
	for (int i = 0; i < tnum; i++)
	{
		m_Nodes[i]->m_Agent->m_Strategy = STRATEGY(rand()%2);
	}

	m_LastUpdateTime = time (NULL);
	m_GenNum = 0;
	fn_calCR();
}