#include "Li_GraphPlayer.h"
#include "../Li_ComnHeader/Li_ComnFunc.h"
#include "../Li_ComnHeader/Li_MathConst.h"

  /*-----------------
 / Graph Functions /
-----------------*/

static int s_multiSelect = 0;	// 0: no multiSelect; 1: draw bounding; 2: moving multipal object

static float s_clkx;
static float s_clky;
static float s_curx;
static float s_cury;

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
			// if single select, the node simply follow the mouse position
			if (s_multiSelect == 0)
			{
				D3DXVECTOR2 newPos = m_CamMan->fn_getAbsPos(s_MousePosAbs);

				m_Nodes[i]->m_PosX = newPos.x;
				m_Nodes[i]->m_PosY = newPos.y;

				// set draw Tip
				drawTip = i;
			}
			// if multi select, draw a circle on it to show those nodes are being selected
			else
			{
				tmpVec2 = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_PosX, m_Nodes[i]->m_PosY));
				fn_DrawCircle(tmpVec2.x, tmpVec2.y, nodeR+1, 2.0f, 175, 225,0);
			}
			// end if-else
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

	// draw bounding square
	if (s_multiSelect == 1)
		fn_DrawRectangle(s_clkx, s_clky, s_curx - s_clkx, s_cury - s_clky);

	// draw Tip
	if (drawTip != -1)
	{
		// clip nodes
		tmpVec = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[drawTip]->m_PosX, m_Nodes[drawTip]->m_PosY));

		// Plot Node Info
		RECT rct;
		rct.left	= (long)tmpVec.x + 20;
		rct.right	= rct.left + 175;
		rct.top		= (long)tmpVec.y + 10;
		rct.bottom	= rct.top + 145;

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
		strcat_s(tmpStr, "\nTtlPayoff : ");
		strcat_s(tmpStr, ParseStr(m_Nodes[drawTip]->m_Agent->m_TotalPayoff, 4));
		strcat_s(tmpStr, "\nAvgPayoff : ");
		strcat_s(tmpStr, ParseStr(m_Nodes[drawTip]->m_Agent->m_AveragePayoff, 4));
		m_font->DrawText(m_dxSpriteInterface,  tmpStr, -1, NULL, DT_LEFT, D3DCOLOR(0xFF000000));
	}
	// end if
}

void Li_GraphPlayer::fn_drawUI()
{
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
	
	strcpy_s(tmpStr, "Press 'ESC' to quit, press '1/2/3' to run/pause/stop the evolution\n");
	strcat_s(tmpStr, "Left Btn: move the Node. Right Btn: move the scene. Mouse wheel: Scale.");
	m_font->DrawText(m_dxSpriteInterface,  tmpStr, -1, NULL, DT_CENTER, D3DCOLOR(0xFFFF0F0F));

	// draw button
	m_btnRun.fn_drawButton(m_dxSpriteInterface);
	m_btnPause.fn_drawButton(m_dxSpriteInterface);
	m_btnStop.fn_drawButton(m_dxSpriteInterface);
	m_btnDisplay.fn_drawButton(m_dxSpriteInterface);
	m_btnLog.fn_drawButton(m_dxSpriteInterface);
}



  /*---------------------
 / FrameWork Functions /
---------------------*/

HRESULT Li_GraphPlayer::fn_ctrl_initD3D()
{
	HRESULT r = Li_GraphBrowser::fn_ctrl_initD3D();

	//// initial new d3d obj here ////
	m_btnRun.fn_setTexture(m_d3dDev, "../res/btnRun.png");
	m_btnRun.fn_setbtnState(IDLE);
	m_btnRun.fn_setPosition(m_WinWidth / 2.0f - 112.0f, m_WinHeight-48.0f);
	
	m_btnPause.fn_setTexture(m_d3dDev, "../res/btnPause.png");
	m_btnPause.fn_setbtnState(IDLE);
	m_btnPause.fn_setPosition(m_WinWidth / 2.0f - 64.0f, m_WinHeight-48.0f);
	
	m_btnStop.fn_setTexture(m_d3dDev, "../res/btnStop.png");
	m_btnStop.fn_setbtnState(IDLE);
	m_btnStop.fn_setPosition(m_WinWidth / 2.0f - 16.0f, m_WinHeight-48.0f);
	
	m_btnDisplay.fn_setTexture(m_d3dDev, "../res/btnDisplay.png");
	m_btnDisplay.fn_setbtnState(PUSH);
	m_btnDisplay.fn_setPosition(m_WinWidth / 2.0f + 32.0f, m_WinHeight-48.0f);
	
	m_btnLog.fn_setTexture(m_d3dDev, "../res/btnLog.png");
	m_btnLog.fn_setbtnState(PUSH);
	m_btnLog.fn_setPosition(m_WinWidth / 2.0f + 80.0f, m_WinHeight-48.0f);

	return r;
}

HRESULT Li_GraphPlayer::fn_ctrl_releaseD3D()
{
	//// release new d3d obj here ////

	return Li_GraphBrowser::fn_ctrl_releaseD3D();
}

void Li_GraphPlayer::fn_ctrl_mainLogic() // in fn_MsgLoop()
{

	if (m_state == PAUSE)
	{
	}
	else if (m_state == INIT)
	{
		fn_initStrategy();
		m_state = PAUSE;
	}

	if (m_state == RUNNING)
	{
		fn_goEpic();
	}
}

void Li_GraphPlayer::fn_ctrl_d3dRender()
{
	// draw graph
	if (m_displayGraph)
		fn_DrawGraph();
	// draw UI
	fn_drawUI();
}

void Li_GraphPlayer::fn_goEpic()
{
	DWORD curtime = GetTickCount();
	if (curtime - m_LastUpdateTime > g_UpdateTime)
	{
		if(m_logGen && m_GenNum == 0)
			fn_log();
		fn_updateGen();
		fn_calPayoff();
		fn_calCR();
		if (m_logGen)
			fn_log();
		fn_UpdateFrame(); // this will force the program to call fn_ctrl_d3dRender()
		m_LastUpdateTime = curtime;
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
		// get the total payoff
		for (int j = 0; j < tsize; j++)
		{
			fn_Play(m_Nodes[i], m_Nodes[i]->m_Conn[j], false);
		}
		// for the average payoff
		m_Nodes[i]->m_Agent->m_AveragePayoff = m_Nodes[i]->m_Agent->m_TotalPayoff / tsize;
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

	// Random initialize ////
	/*
	for (int i = 0; i < tnum; i++)
	{
		m_Nodes[i]->m_Agent->m_Strategy = STRATEGY(rand()%2);
	}
	*/

	// to all C
	for (int i = 0; i < tnum; i++)
	{
		m_Nodes[i]->m_Agent->m_Strategy = PUREC;
	}

	m_LastUpdateTime = GetTickCount ();
	m_GenNum = 0;
	fn_calPayoff();
	fn_calCR();
}

bool Li_GraphPlayer::fn_keyListener()
{
	Li_GraphBrowser::fn_keyListener();

	//// Keyboard inputs ////
	static bool keyPushed = false;

	// input 1 to run the graph
	if(m_DxInput->m_KeyState[DIK_1] & 0x80)
	{
		if (!keyPushed)
		{
			m_state = RUNNING;
			keyPushed = true;
		}
	}
	// input 2 to reset the graph
	else if(m_DxInput->m_KeyState[DIK_2] & 0x80)
	{
		if (!keyPushed)
		{
			m_state = PAUSE;
			keyPushed = true;
		}
	}
	// input 3 to pause the graph
	else if(m_DxInput->m_KeyState[DIK_3] & 0x80)
	{
		if (!keyPushed)
		{
			m_state = INIT;
			keyPushed = true;
		}
	}
	else if(m_DxInput->m_KeyState[DIK_C] & 0x80)
	{
		if (!keyPushed)
		{
			// only response the change strategy when we can see the graph
			// just to prevent strategy change by mistakes
			if (m_displayGraph)
			{
				float nodeR = m_nodeSprite.fn_getWidth() / 2.0f;

				// loop from the back to front, make sure always select the one on top
				for (int i = (int)m_Nodes.size() - 1; i >= 0; i--)
				{
					D3DXVECTOR2 tmpVec = D3DXVECTOR2(0,0);
					// clip nodes
					tmpVec = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_PosX, m_Nodes[i]->m_PosY));

					if (((s_MousePosAbs.x - tmpVec.x)*(s_MousePosAbs.x - tmpVec.x) +
						(s_MousePosAbs.y - tmpVec.y)*(s_MousePosAbs.y - tmpVec.y) <= nodeR*nodeR))
					{
						m_Nodes[i]->m_Agent->m_Strategy = PUREC;

						fn_calPayoff();
						fn_calCR();

						// only one can been selected at a time, deal with overlap nodes
						break;
					}
				}
				// end for

				// update frame when the mous button down, so we could move the node in the render loop
				// save from using another loop
				fn_UpdateFrame(); // this will force the program to call fn_ctrl_d3dRender()
			}
			// end display graph
			
			keyPushed = true;
		}
		// end if keypushed
	}
	// End DIK_C
	else if(m_DxInput->m_KeyState[DIK_D] & 0x80)
	{
		if (!keyPushed)
		{
			// only response the change strategy when we can see the graph
			// just to prevent strategy change by mistakes
			if (m_displayGraph)
			{
				float nodeR = m_nodeSprite.fn_getWidth() / 2.0f;

				// loop from the back to front, make sure always select the one on top
				for (int i = (int)m_Nodes.size() - 1; i >= 0; i--)
				{
					D3DXVECTOR2 tmpVec = D3DXVECTOR2(0,0);
					// clip nodes
					tmpVec = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_PosX, m_Nodes[i]->m_PosY));

					if (((s_MousePosAbs.x - tmpVec.x)*(s_MousePosAbs.x - tmpVec.x) +
						(s_MousePosAbs.y - tmpVec.y)*(s_MousePosAbs.y - tmpVec.y) <= nodeR*nodeR))
					{
						m_Nodes[i]->m_Agent->m_Strategy = PURED;

						fn_calPayoff();
						fn_calCR();

						// only one can been selected at a time, deal with overlap nodes
						break;
					}
				}
				// end for

				// update frame when the mous button down, so we could move the node in the render loop
				// save from using another loop
				fn_UpdateFrame(); // this will force the program to call fn_ctrl_d3dRender()
			}
			// end display graph
			
			keyPushed = true;
		}
		// end if keypushed
	}
	// End DIK_D
	else if(m_DxInput->m_KeyState[DIK_G] & 0x80)
	{
		if (!keyPushed)
		{
			// only response the change strategy when we can see the graph
			// just to prevent strategy change by mistakes
			if (m_displayGraph)
			{
				float nodeR = m_nodeSprite.fn_getWidth() / 2.0f;

				int s = -1;
				// loop from the back to front, make sure always select the one on top
				for (int i = (int)m_Nodes.size() - 1; i >= 0; i--)
				{
					D3DXVECTOR2 tmpVec = D3DXVECTOR2(0,0);
					// clip nodes
					tmpVec = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_PosX, m_Nodes[i]->m_PosY));

					if (((s_MousePosAbs.x - tmpVec.x)*(s_MousePosAbs.x - tmpVec.x) +
						(s_MousePosAbs.y - tmpVec.y)*(s_MousePosAbs.y - tmpVec.y) <= nodeR*nodeR))
					{
						s = i;
						// only one can been selected at a time, deal with overlap nodes
						break;
					}
				}
				// end for

				if (s >= 0)
				{
					int radius = 25 + m_Nodes[s]->m_Conn.size()*2;
					// move all of the neighbours of current selected
					for (int i = 0; i < (int)m_Nodes[s]->m_Conn.size(); i++)
					{
						m_Nodes[s]->m_Conn[i]->m_PosX = m_Nodes[s]->m_PosX + radius *(float)cos(2 * PI * i / m_Nodes[s]->m_Conn.size());
						m_Nodes[s]->m_Conn[i]->m_PosY = m_Nodes[s]->m_PosY + radius *(float)sin(2 * PI * i / m_Nodes[s]->m_Conn.size());
					}
				}

				// update frame when the mous button down, so we could move the node in the render loop
				// save from using another loop
				fn_UpdateFrame(); // this will force the program to call fn_ctrl_d3dRender()
			}
			// end display graph
			
			keyPushed = true;
		}
		// end if keypushed
	}
	// End DIK_G
	else // default, push nothing or useless key
		keyPushed = false;

	return false;
}

bool Li_GraphPlayer::fn_mouseListener()
{
	static bool lbd = false;
	//// drag the node ////
	if (s_multiSelect == 0) // click right on the node
	{
		static bool alreadySelectedANode = false;
		if (s_isLBtnDown)
		{
			lbd = true;
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

				// if didn't click on the node, start to draw multi selection bounding
				// record the click position
				if (!alreadySelectedANode)
				{
					s_clkx = s_MousePosAbs.x;
					s_clky = s_MousePosAbs.y;
					s_curx = s_clkx;
					s_cury = s_clky;
					s_multiSelect = 1;
				}
			}
			// end if
		}
		// mouse left up
		else if (lbd) //if (s_isLRelease)
		{
			lbd = false;
			for (unsigned int i = 0; i < m_Nodes.size(); i++)
			{
				// release all
				m_Nodes[i]->m_isSelected = false;
			}
			alreadySelectedANode = false;
			// end for
		}
		// end if
	}
	// now start multiselect
	// 1. draw multi selection bounding
	else if (s_multiSelect == 1)
	{
		if (s_isLBtnDown)
		{
			lbd = true;

			s_curx = s_MousePosAbs.x;
			s_cury = s_MousePosAbs.y;

			// get the bounding
			float top, btm, lft, rht;
			if (s_clkx <= s_curx)
			{
				lft = s_clkx;
				rht = s_curx;
			}
			else
			{
				lft = s_curx;
				rht = s_clkx;
			}

			if (s_clky <= s_cury)
			{
				top = s_clky;
				btm = s_cury;
			}
			else
			{
				top = s_cury;
				btm = s_clky;
			}
			
			// go throung every node, to see if it in the bounding
			for (int i = 0; i < (int)m_Nodes.size(); i++)
			{
				D3DXVECTOR2 tmpVec = D3DXVECTOR2(0,0);
				// clip nodes
				tmpVec = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_PosX, m_Nodes[i]->m_PosY));


				// not test if the nodes in the bounding
				if (tmpVec.x >= lft && tmpVec.x <= rht &&
					tmpVec.y >= top && tmpVec.y <= btm)
				{
					m_Nodes[i]->m_isSelected = true;
				}
				else
				{
					// prevent roll back, to put thos unselected one unselected
					m_Nodes[i]->m_isSelected = false;
				}
			}
			// end for
		}
		else if (lbd) //if (s_isLRelease)
		{
			lbd = false;
			bool hassellected = false;

			for (int i = 0; i < (int)m_Nodes.size(); i++)
			{
				if (m_Nodes[i]->m_isSelected)
				{
					hassellected = true;
					break; // save time
				}
			}
			// end for

			if (!hassellected)
			{
				s_clkx = s_MousePosAbs.x;
				s_clky = s_MousePosAbs.y;
				s_curx = s_clkx;
				s_cury = s_clky;
				s_multiSelect = 0;
			}
			else
			{
				s_multiSelect = 2;
			}
		}
		// end if else
	}
	// 2. move multi nodes
	else if (s_multiSelect == 2)
	{
		if (s_isLBtnDown && !lbd) //(s_isLClick)
		{
			lbd = true;
			s_clkx = s_MousePosAbs.x;
			s_clky = s_MousePosAbs.y;
		}
		else if (s_isLBtnDown)
		{
			s_curx = s_MousePosAbs.x;
			s_cury = s_MousePosAbs.y;
			
				// move all nodes that are selected
			for (unsigned int i = 0; i < m_Nodes.size(); i++)
			{
				if (m_Nodes[i]->m_isSelected)
				{
					m_Nodes[i]->m_PosX += (s_curx - s_clkx) / m_CamMan->m_CameraScale;
					m_Nodes[i]->m_PosY += (s_cury - s_clky) / m_CamMan->m_CameraScale;
				}
			}

			s_clkx = s_curx;
			s_clky = s_cury;
		}
		else if (lbd)//if (s_isLRelease)
		{
			lbd = false;
			for (unsigned int i = 0; i < m_Nodes.size(); i++)
			{
				// release all
				m_Nodes[i]->m_isSelected = false;
			}

			s_multiSelect = 0;
			// end for
		}
	}
	//// end drag the node ////

	//// Mouse Left Button ////
	// Button events
	// run button
	if (s_MousePosAbs.x > m_btnRun.fn_getx() && s_MousePosAbs.x < m_btnRun.fn_getx() + m_btnRun.fn_getWidth() &&
		s_MousePosAbs.y > m_btnRun.fn_gety() && s_MousePosAbs.y < m_btnRun.fn_gety() + m_btnRun.fn_getHeight())
	{
		m_btnRun.fn_setbtnState(PASS);

		if (s_isLBtnDown)
		{
			m_btnRun.fn_setbtnState(PUSH);
		}
		else if (s_isLRelease)
		{
			m_state = RUNNING;
		}
	}
	else
	{
		m_btnRun.fn_setbtnState(IDLE);
	}

	// pause button
	if (s_MousePosAbs.x > m_btnPause.fn_getx() && s_MousePosAbs.x < m_btnPause.fn_getx() + m_btnPause.fn_getWidth() &&
		s_MousePosAbs.y > m_btnPause.fn_gety() && s_MousePosAbs.y < m_btnPause.fn_gety() + m_btnPause.fn_getHeight())
	{
		m_btnPause.fn_setbtnState(PASS);

		if (s_isLBtnDown)
		{
			m_btnPause.fn_setbtnState(PUSH);
		}
		else if (s_isLRelease)
		{
			m_state = PAUSE;
		}
	}
	else
	{
		m_btnPause.fn_setbtnState(IDLE);
	}

	// stop button
	if (s_MousePosAbs.x > m_btnStop.fn_getx() && s_MousePosAbs.x < m_btnStop.fn_getx() + m_btnStop.fn_getWidth() &&
		s_MousePosAbs.y > m_btnStop.fn_gety() && s_MousePosAbs.y < m_btnStop.fn_gety() + m_btnStop.fn_getHeight())
	{
		m_btnStop.fn_setbtnState(PASS);

		if (s_isLBtnDown)
		{
			m_btnStop.fn_setbtnState(PUSH);
		}
		else if (s_isLRelease)
		{
			m_state = INIT;
		}
	}
	else
	{
		m_btnStop.fn_setbtnState(IDLE);
	}

	// Display button
	if (s_MousePosAbs.x > m_btnDisplay.fn_getx() && s_MousePosAbs.x < m_btnDisplay.fn_getx() + m_btnDisplay.fn_getWidth() &&
		s_MousePosAbs.y > m_btnDisplay.fn_gety() && s_MousePosAbs.y < m_btnDisplay.fn_gety() + m_btnDisplay.fn_getHeight())
	{
		if (s_isLClick)
		{
			if (m_btnDisplay.fn_getState() == IDLE)
			{
				m_btnDisplay.fn_setbtnState(PUSH);
				m_displayGraph = true;
			}
			else
			{
				m_btnDisplay.fn_setbtnState(IDLE);
				m_displayGraph = false;
			}
		}
	}

	// Log button
	if (s_MousePosAbs.x > m_btnLog.fn_getx() && s_MousePosAbs.x < m_btnLog.fn_getx() + m_btnLog.fn_getWidth() &&
		s_MousePosAbs.y > m_btnLog.fn_gety() && s_MousePosAbs.y < m_btnLog.fn_gety() + m_btnLog.fn_getHeight())
	{
		if (s_isLClick)
		{
			if (m_btnLog.fn_getState() == IDLE)
			{
				m_btnLog.fn_setbtnState(PUSH);
				m_logGen = true;
			}
			else
			{
				m_btnLog.fn_setbtnState(IDLE);
				m_logGen = false;
			}
		}
	}
	// end buttons ////

	return false;
}

void Li_GraphPlayer::fn_log()
{
	m_fout<<m_GenNum<<". "<<m_CR<<endl;
}