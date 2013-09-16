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

static int s_expNum = 0;
static int s_maxExpNum = 500;
static int s_maxGen = 50;

void Li_GraphPlayer::fn_DrawGraph()
{
	int num = m_Nodes.size();
	float nodeR			= 0.0f;
	D3DXVECTOR2 tmpVec	= D3DXVECTOR2(0,0);	// the rel pos for node i
	D3DXVECTOR2 tmpVec2	= D3DXVECTOR2(0,0); // the rel pos for note i 's connection node i->j
	int drawTip			= -1;

	// draw nodes and edges
	for (int i = 0; i < num; i++)
	{
		// clip nodes
		tmpVec = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_PosX, m_Nodes[i]->m_PosY));

		// get texture radius
		nodeR = m_nodeSprite.fn_getWidth() / 2.0f;

		//// Draw Nodes ////

		// only draw nodes if its in the current camera view
		if (tmpVec.x > -nodeR && tmpVec.y > -nodeR && tmpVec.x < m_WinWidth+nodeR && tmpVec.y < m_WinHeight+nodeR)
		{
			// node is grabed
			if (m_Nodes[i]->m_isSelected)
			{
				// if single select, the node simply follow the mouse position
				if (s_multiSelect == 0)
				{
					// test if the mouse actually move outside the node
					// just for not mass the graph, the graph only move when the mouse move
					//D3DXVECTOR2 newPos = m_CamMan->fn_getAbsPos(s_MousePosAbs);

					//m_Nodes[i]->m_PosX = newPos.x;
					//m_Nodes[i]->m_PosY = newPos.y;
				
					if (s_isLBtnDown)
					{
						m_Nodes[i]->m_PosX += (s_curx - s_clkx) / m_CamMan->m_CameraScale;
						m_Nodes[i]->m_PosY += (s_cury - s_clky) / m_CamMan->m_CameraScale;
				
						s_clkx = s_curx;
						s_clky = s_cury;
					}

					// set draw Tip
					drawTip = i;
				}
				// if multi select, draw a circle on it to show those nodes are being selected
				else
				{
					tmpVec2 = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_PosX, m_Nodes[i]->m_PosY));
					fn_DrawCircle(tmpVec2.x, tmpVec2.y, nodeR+1, 2.0f, 175, 225,0);
				
					// keep the mouse in the mouse lisenter, since it not make the program too slow,
					// but could get rid of the delay of the draw connection on the graphic
					// (because the move is done before draw connection, not during that)
					/*
					// move multiply nodes
					if (s_multiSelect == 2)
					{
						if (s_isLBtnDown)
						{
							m_Nodes[i]->m_PosX += (s_curx - s_clkx) / m_CamMan->m_CameraScale;
							m_Nodes[i]->m_PosY += (s_cury - s_clky) / m_CamMan->m_CameraScale;
						}
					}
					*/
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
		}
		// end if


		//// Draw Connections ////

		// only draw the connections if the size is not too big.
		if (m_Nodes.size() <= 5000)
		{
			// draw connections
			for (unsigned int j = 0; j < m_Nodes[i]->m_Conn.size(); j++)
			{
				tmpVec2 = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_Conn[j]->m_PosX, m_Nodes[i]->m_Conn[j]->m_PosY));

				// calculate the length of the line, if it too short, dont draw it
				float tmplength = sqrt((tmpVec.x - tmpVec2.x)*(tmpVec.x - tmpVec2.x) + (tmpVec.y - tmpVec2.y)*(tmpVec.y - tmpVec2.y));
				
				// also, dont draw the line if both of its node are out side of the camera view
				if (tmplength >= nodeR * 2)
					// && ((tmpVec.x > -nodeR && tmpVec.y > -nodeR && tmpVec.x < m_WinWidth+nodeR && tmpVec.y < m_WinHeight+nodeR) ||
					//(tmpVec2.x > -nodeR && tmpVec2.y > -nodeR && tmpVec2.x < m_WinWidth+nodeR && tmpVec2.y < m_WinHeight+nodeR)))
				{
					// if the node i is selected, draw all of the lines to its connection nodes blue
					if (m_Nodes[i]->m_isSelected)
					{
						fn_DrawLine(tmpVec.x, tmpVec.y, tmpVec2.x, tmpVec2.y, 2, 0, 0, 255);
					}
					else if (m_Nodes[i]->m_Conn[j]->m_ID > m_Nodes[i]->m_ID)
					{
						// if node i is not selected
						// print it red by default, if the mouse on the node, print it green
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
					// end if (m_Nodes[i]->m_isSelected)
				}
				// end if (tmplength >= 100)	
			}
			// end for j
		}
	}
	// end for i

	// keep the mouse in the mouse lisenter, since it not make the program too slow,
	// but could get rid of the delay of the draw connection on the graphic
	// (because the move is done before draw connection, not during that)
	// update the mose current position after going through every nodes in multiple selection mode
	// so that we could move all the selected nodes together
	/*
	if (s_multiSelect == 2)
	{
		if (s_isLBtnDown)
		{
			s_clkx = s_curx;
			s_clky = s_cury;
		}
	}
	*/

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
	
	char tmpStr[512];

	// if the file name have been assigned
	if (!m_Nodes.empty())
	{
		rct.left	= m_WinWidth - 375;
		rct.right	= m_WinWidth - 20;
		rct.top		= 20;
		rct.bottom	= 180;

		// draw m_textBoxSprite
		m_textBoxSprite.fn_drawAsBoarder(m_dxSpriteInterface, rct);
		strcpy_s(tmpStr, "The Graph Attributes: ");
		strcat_s(tmpStr, "\nThe Graph Size: ");
		strcat_s(tmpStr, ParseStr(m_GraphSize));
		strcat_s(tmpStr, "\nAverage Degree: ");
		strcat_s(tmpStr, ParseStr(m_AvgDegree, 6));
		strcat_s(tmpStr, "\nTransitivity: ");
		strcat_s(tmpStr, ParseStr(m_Transitivity, 6));
		strcat_s(tmpStr, "\nAverage Clustering Coefficient: ");
		strcat_s(tmpStr, ParseStr(m_AvgCC, 6));
		strcat_s(tmpStr, "\nGeneration Number: ");
		strcat_s(tmpStr, ParseStr(m_GenNum));
		strcat_s(tmpStr, "\nCooperation Rate: ");
		strcat_s(tmpStr, ParseStr(m_CR, 6));
		m_font->DrawText(m_dxSpriteInterface,  tmpStr, -1, NULL, DT_LEFT, D3DCOLOR(0xFF000000));
	}

	/*
	// user manu
	rct.left = m_WinWidth / 2 - 310;
	rct.right = m_WinWidth / 2 + 310;
	rct.top = m_WinHeight - 150;
	rct.bottom = m_WinHeight - 50;

	// draw m_textBoxSprite
	m_textBoxSprite.fn_drawAsBoarder(m_dxSpriteInterface, rct);
	
	strcpy_s(tmpStr, "'ESC':quit, '1/2/3':run/pause/stop, 'G':gather nodes, 'Del':remove node \n");
	strcat_s(tmpStr, "Hold 'Shift':new node, Hold 'Ctrl':new edge, Hold 'Alt':remove edge \n");
	strcat_s(tmpStr, "Left Btn: move the Node. Right Btn: move the scene. Mouse wheel: Scale. \n");
	strcat_s(tmpStr, "Double Click(or C/D): change strategy to C or D.");
	strcat_s(tmpStr, " FPS : ");
	strcat_s(tmpStr, ParseStr(m_FPS, 6));
	m_font->DrawText(m_dxSpriteInterface,  tmpStr, -1, NULL, DT_CENTER, D3DCOLOR(0xFFFF0F0F));
	*/
	rct.left = m_WinWidth - 150;
	rct.right = m_WinWidth - 5;
	rct.top = m_WinHeight - 50;
	rct.bottom = m_WinHeight - 5;
	// draw m_textBoxSprite
	m_textBoxSprite.fn_drawAsBoarder(m_dxSpriteInterface, rct);
	strcpy_s(tmpStr, " FPS : ");
	strcat_s(tmpStr, ParseStr(m_FPS, 2));
	m_font->DrawText(m_dxSpriteInterface,  tmpStr, -1, NULL, DT_CENTER, D3DCOLOR(0xFFFF0F0F));

	// draw button
	m_btnRun.fn_drawButton(m_dxSpriteInterface);
	m_btnPause.fn_drawButton(m_dxSpriteInterface);
	m_btnStop.fn_drawButton(m_dxSpriteInterface);
	m_btnDisplay.fn_drawButton(m_dxSpriteInterface);
	m_btnLog.fn_drawButton(m_dxSpriteInterface);
	m_btnNew.fn_drawButton(m_dxSpriteInterface);
	m_btnOpen.fn_drawButton(m_dxSpriteInterface);
	m_btnSave.fn_drawButton(m_dxSpriteInterface);
	m_btnSaveMatrix.fn_drawButton(m_dxSpriteInterface);

	// draw the list button
	m_lstBtnGraphLayout.fn_drawButton(m_dxSpriteInterface);
	if (m_lstBtnGraphLayout.fn_getState() == PASS)
		m_lstBtnGraphLayout.fn_drawButtonList(m_dxSpriteInterface);
}



  /*---------------------
 / FrameWork Functions /
---------------------*/

HRESULT Li_GraphPlayer::fn_ctrl_initD3D()
{
	HRESULT r = Li_FW::fn_ctrl_initD3D();

	//// initial new d3d obj here ////
	// set Node texture
	m_nodeSprite.fn_setTexture(m_d3dDev, "../res/sdk_handle.png");
	// set textbox texture
	m_textBoxSprite.fn_setTexture(m_d3dDev, "../res/sdk_mini_text_box.png");
	//// initial new d3d obj here ////

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
	m_btnLog.fn_setbtnState(IDLE);
	m_btnLog.fn_setPosition(m_WinWidth / 2.0f + 80.0f, m_WinHeight-48.0f);
	
	m_btnNew.fn_setTexture(m_d3dDev, "../res/btnNew.png");
	m_btnNew.fn_setbtnState(IDLE);
	m_btnNew.fn_setPosition(50.0f, 10.0f);
	
	m_btnOpen.fn_setTexture(m_d3dDev, "../res/btnOpen.png");
	m_btnOpen.fn_setbtnState(IDLE);
	m_btnOpen.fn_setPosition(90.0f, 10.0f);

	m_btnSave.fn_setTexture(m_d3dDev, "../res/btnSave.png");
	m_btnSave.fn_setbtnState(IDLE);
	m_btnSave.fn_setPosition(130.0f, 10.0f);

	m_btnSaveMatrix.fn_setTexture(m_d3dDev, "../res/btnSaveMatrix.png");
	m_btnSaveMatrix.fn_setbtnState(IDLE);
	m_btnSaveMatrix.fn_setPosition(170.0f, 10.0f);

	m_lstBtnGraphLayout.fn_setTexture(m_d3dDev, "../res/lstBtnGraphLayout.png");
	m_lstBtnGraphLayout.fn_setPosition(10.0f, 10.0f);

	return r;
}

HRESULT Li_GraphPlayer::fn_ctrl_releaseD3D()
{
	//// release new d3d obj here ////
	// release Node texture
	m_nodeSprite.fn_releaseTexture();
	// release textbox texture
	m_textBoxSprite.fn_releaseTexture();
	//// release new d3d obj here ////

	// release button objects ////
	m_btnRun.fn_releaseTexture();
	m_btnPause.fn_releaseTexture();
	m_btnStop.fn_releaseTexture();
	m_btnDisplay.fn_releaseTexture();
	m_btnLog.fn_releaseTexture();
	m_btnNew.fn_releaseTexture();
	m_btnOpen.fn_releaseTexture();
	m_btnSave.fn_releaseTexture();
	m_btnSaveMatrix.fn_releaseTexture();
	m_lstBtnGraphLayout.fn_releaseTexture();

	return Li_FW::fn_ctrl_releaseD3D();
}

void Li_GraphPlayer::fn_ctrl_mainLogic() // in fn_MsgLoop()
{
	int num = m_Nodes.size();

	// moving the nodes with animation
	// Time control
	// currenttime and lasttime is controled while rendering frame
	// here, we only use the value of them, animated as the same time as rendering in each frame
	// 在渲染框架中控制时间的变化，获取，更改上次变动时间及当前时间，这里只读数，以便在每一帧的渲染中播放一帧动画
	if (m_currTime - m_lastTime >= m_Win->iTimeDelta)
	{
		if (m_inAnimation)
		{
			bool isallmoved = true;
			for (int i = 0; i < num; i++)
			{
				float diffx = m_Nodes[i]->m_DestX - m_Nodes[i]->m_PosX;
				float diffy = m_Nodes[i]->m_DestY - m_Nodes[i]->m_PosY;
				float diff = sqrt(diffx*diffx + diffy*diffy);
				float tmpSpd = 10;
				float movex, movey;
				if (diffx != 0)
				{
					movex = tmpSpd * diffx/diff;
					if (diffx > 0)
					{
						if (diffx >= movex)
							m_Nodes[i]->m_PosX += movex;
						else
							m_Nodes[i]->m_PosX += diffx;
					}
					else if (diffx < 0)
					{
						if (diffx <= movex)
							m_Nodes[i]->m_PosX += movex;
						else
							m_Nodes[i]->m_PosX += diffx;
					}
				}
				// end if x
				if (diffy != 0)
				{
					movey = tmpSpd * diffy/diff;
					if (diffy > 0)
					{
						if (diffy >= movey)
							m_Nodes[i]->m_PosY += movey;
						else
							m_Nodes[i]->m_PosY += diffy;
					}
					else if (diffy < 0)
					{
						if (diffy <= movey)
							m_Nodes[i]->m_PosY += movey;
						else
							m_Nodes[i]->m_PosY += diffy;
					}
				}
				// end if y

				// if move to the destination, set animation false
				if (diffx != 0 || diffy != 0)
					isallmoved = false;
			}
			// end for

			if (isallmoved)
				m_inAnimation = false;

			// dont forget update frame
			fn_UpdateFrame();
		}
		else
		{
			for (int i = 0; i < num; i++)
			{
				m_Nodes[i]->m_DestX = m_Nodes[i]->m_PosX;
				m_Nodes[i]->m_DestY = m_Nodes[i]->m_PosY;
			}
		}
	}

	if (m_state == PAUSE)
	{
	}
	else if (m_state == INIT)
	{
		s_expNum = 0;
		fn_initStrategy();
		m_Nodes[rand()%m_Nodes.size()]->m_Agent->m_Strategy = PURED;
		fn_calPayoff();
		fn_calCR();
		m_state = PAUSE;
	}

	if (m_state == RUNNING)
	{
		// control the game progress
		// running
		if (m_GenNum < s_maxGen)
			fn_goEpic();
		else
		{
			fn_initStrategy();
			m_Nodes[rand()%m_Nodes.size()]->m_Agent->m_Strategy = PURED;
			fn_calPayoff();
			fn_calCR();
			s_expNum++;
		}

		if (s_expNum == s_maxExpNum)
		{
			s_expNum = 0;
			m_state = PAUSE;
		}
	}
}

void Li_GraphPlayer::fn_ctrl_d3dRender()
{
	// draw graph
	if (m_displayGraph && !m_Nodes.empty())
		fn_DrawGraph();
	// draw UI
	fn_drawUI();
}

void Li_GraphPlayer::fn_goEpic()
{
	if (!m_Nodes.empty())
	{
		DWORD curtime = GetTickCount();

		// if dont display the graph, just go as fast as it can
		// else, keep a constant frame to run
		DWORD realupdatetime;
		if (m_displayGraph)
			realupdatetime = g_UpdateTime;
		else
			realupdatetime = 0;

		if (curtime - m_LastUpdateTime > realupdatetime)
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
	// end if
}

void Li_GraphPlayer::fn_calPayoff()
{
	if (!m_Nodes.empty())
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
			if (!m_Nodes[i]->m_Conn.empty())
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
			//end if
		}
		// end for
	}
	// end if
}

// calculate cooperation rate
void Li_GraphPlayer::fn_calCR()
{
	if (!m_Nodes.empty())
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
	// end if
}

void Li_GraphPlayer::fn_updateGen()
{
	if (!m_Nodes.empty())
	{
		// 1. adopt new strategy
		// in this step, the new strategy is saved in mUpdateStrategy
		int tnum = m_Nodes.size();
		for (int i = 0; i < tnum; i++)
		{
			// normal learning
			//fn_Evolve(m_Nodes[i]);
			// weighted learning
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
	// end if
}

void Li_GraphPlayer::fn_initStrategy()
{
	if (!m_Nodes.empty())
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
	// end if
}

bool Li_GraphPlayer::fn_keyListener()
{
	//// Keyboard inputs ////
	// input R to reinit the graph
	if(m_DxInput->m_KeyState[DIK_R] & 0x80)
	{
		if (!m_Nodes.empty())
		{
			if (m_lstBtnGraphLayout.fn_getCurrentSelected() == 0)
			{
				if (strcmp(m_gpName, "") != 0)
				{
					fn_DestroyGraph();
					fn_LoadGraph();
					fn_calPayoff();
					fn_calCR();

					if (m_logGen)
						fn_setLogFile();
				}
			}
			else if (m_lstBtnGraphLayout.fn_getCurrentSelected() == 1)
				fn_InitNodePos(L_CIRCLE);
			else if (m_lstBtnGraphLayout.fn_getCurrentSelected() == 2)
				fn_InitNodePos(L_SQUARE);

			fn_UpdateFrame();
		}
		// end if
	}

	// input ESC to quit
	if(m_DxInput->m_KeyState[DIK_ESCAPE] & 0x80)
		PostQuitMessage(0);
	//// Keyboard inputs ////

	//// Keyboard inputs ////
	static bool keyPushed = false;

	// input 1 to run the graph
	if(m_DxInput->m_KeyState[DIK_1] & 0x80)
	{
		if (!keyPushed)
		{
			if (!m_Nodes.empty())
			{
				m_state = RUNNING;
			}
			// end if
			keyPushed = true;
		}
	}
	// input 2 to reset the graph
	else if(m_DxInput->m_KeyState[DIK_2] & 0x80)
	{
		if (!keyPushed)
		{
			if (!m_Nodes.empty())
			{
				m_state = PAUSE;
			}
			// end if
			keyPushed = true;
		}
	}
	// input 3 to pause the graph
	else if(m_DxInput->m_KeyState[DIK_3] & 0x80)
	{
		if (!keyPushed)
		{
			if (!m_Nodes.empty())
			{
				m_state = INIT;
			}
			// end if
			keyPushed = true;
		}
	}
	else if(m_DxInput->m_KeyState[DIK_C] & 0x80)
	{
		if (!keyPushed)
		{
			if (!m_Nodes.empty())
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
			}
			// end if
			
			keyPushed = true;
		}
		// end if keypushed
	}
	// End DIK_C
	else if(m_DxInput->m_KeyState[DIK_D] & 0x80)
	{
		if (!keyPushed)
		{
			if (!m_Nodes.empty())
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
			}
			// end if
			
			keyPushed = true;
		}
		// end if keypushed
	}
	// End DIK_D
	else if(m_DxInput->m_KeyState[DIK_G] & 0x80)
	{
		if (!keyPushed)
		{
			if (!m_Nodes.empty())
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
							//m_Nodes[s]->m_Conn[i]->m_PosX = m_Nodes[s]->m_PosX + radius *(float)cos(2 * PI * i / m_Nodes[s]->m_Conn.size());
							//m_Nodes[s]->m_Conn[i]->m_PosY = m_Nodes[s]->m_PosY + radius *(float)sin(2 * PI * i / m_Nodes[s]->m_Conn.size());
							m_Nodes[s]->m_Conn[i]->m_DestX = m_Nodes[s]->m_PosX + radius *(float)cos(2 * PI * i / m_Nodes[s]->m_Conn.size());
							m_Nodes[s]->m_Conn[i]->m_DestY = m_Nodes[s]->m_PosY + radius *(float)sin(2 * PI * i / m_Nodes[s]->m_Conn.size());
							m_inAnimation = true;
						}
					}

					// update frame when the mous button down, so we could move the node in the render loop
					// save from using another loop
					fn_UpdateFrame(); // this will force the program to call fn_ctrl_d3dRender()
				}
				// end display graph
			}
			// end if
			
			keyPushed = true;
		}
		// end if keypushed
	}
	// End DIK_G
	else if(m_DxInput->m_KeyState[DIK_M] & 0x80)
	{
		if (!keyPushed)
		{
			if (!m_Nodes.empty())
			{
				// 用来记录已经整合位置的点的列表
				// 因为评测共邻比较浪费计算周期，所以记录已经整合的点，节省计算时间
				vector<bool> ismerged;

				int num = m_Nodes.size();

				for (int i = 0; i < num; i++)
				{
					ismerged.push_back(false);
				}

				for (int i = 0; i < num - 1; i++)
				{
					// 如果未整合过
					if (!ismerged[i])
					{
						// 即便未整合过，i点也已经被前面的点测试过，所以不用考虑i前面的点
						for (int j = i+1; j < num; j++)
						{
							if (!ismerged[j])
							{
								// 先只考虑直连共邻， 把2改为1则是非直连共邻，1，2同时则是任意共邻
								if (m_Nodes[i]->fn_neighbourSharingTo(m_Nodes[j]) == 2)
								{
									// 共邻的情况下，先转移坐标，然后将 j 设置为已经整合过
									// 此处不用设置i, 在i循环的结尾统一设置i的整合状态为真
									m_Nodes[j]->m_DestX = m_Nodes[i]->m_PosX;
									m_Nodes[j]->m_DestY = m_Nodes[i]->m_PosY;
									ismerged[j] = true;
									// 设置移动动画开启
									m_inAnimation = true;
								}
								// end if
							}
							// end if j
						}
						// end for j
					}
					// end if i
					ismerged[i] = true;
				}
				// end for i
			}
			// end if
			
			keyPushed = true;
		}
		// end if keypushed
	}
	// End DIK_M
	else // default, push nothing or useless key
		keyPushed = false;

	return false;
}

bool Li_GraphPlayer::fn_mouseListener()
{
	fn_EditGraph();

	// no need for node detection for empyt graph
	if (!m_Nodes.empty())
	{
		// record the left button state, because the win message is an sync thread, 
		// the message is not very reliable for the time delay
		static bool lbd = false;
		//// drag the node ////
		if (s_multiSelect == 0)
		{
			static bool alreadySelectedANode = false;

			if (s_isLBtnDown && !lbd) //(s_isLClick)
			{
				lbd = true;
				s_clkx = s_MousePosAbs.x;
				s_clky = s_MousePosAbs.y;
				s_curx = s_clkx;
				s_cury = s_clky;
			}
			else if (s_isLBtnDown)
			{
				lbd = true;
				s_curx = s_MousePosAbs.x;
				s_cury = s_MousePosAbs.y;

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
							(!alreadySelectedANode)) // click right on the node
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
				s_curx = s_clkx;
				s_cury = s_clky;
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

		// double click to change the strategy
		if (s_isLDBClick)
		{
			// to get rid of the delay of the message which could make this method run multiple times
			s_isLDBClick = false;
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
						if (m_Nodes[i]->m_Agent->m_Strategy == PUREC)
							m_Nodes[i]->m_Agent->m_Strategy = PURED;
						else if (m_Nodes[i]->m_Agent->m_Strategy == PURED)
							m_Nodes[i]->m_Agent->m_Strategy = PUREC;

						fn_calPayoff();
						fn_calCR();

						// only one can been selected at a time, deal with overlap nodes
						break;
					}
				}
				// end for
			}
		}
	}
	// end if (!m_Nodes.empty()) ////

	//// Mouse Left Button ////
	// Button events
	// run button
	if (m_btnRun.fn_isMouseOn(s_MousePosAbs))
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
	if (m_btnPause.fn_isMouseOn(s_MousePosAbs))
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
	if (m_btnStop.fn_isMouseOn(s_MousePosAbs))
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
	if (m_btnDisplay.fn_isMouseOn(s_MousePosAbs))
	{
		if (s_isLClick)
		{
			s_isLClick = false;
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
	if (m_btnLog.fn_isMouseOn(s_MousePosAbs))
	{
		if (s_isLClick)
		{
			s_isLClick = false;
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

	// New button
	if (m_btnNew.fn_isMouseOn(s_MousePosAbs))
	{
		m_btnNew.fn_setbtnState(PASS);

		if (s_isLBtnDown)
		{
			m_btnNew.fn_setbtnState(PUSH);
		}
		else if (s_isLRelease)
		{
			fn_DestroyGraph();
		}
	}
	else
	{
		m_btnNew.fn_setbtnState(IDLE);
	}

	// Open button
	if (m_btnOpen.fn_isMouseOn(s_MousePosAbs))
	{
		m_btnOpen.fn_setbtnState(PASS);

		if (s_isLBtnDown)
		{
			m_btnOpen.fn_setbtnState(PUSH);
		}
		else if (s_isLRelease)
		{
			// get graph file name
			strcpy_s(m_gpName, sizeof(m_gpName), openfilename());

			// test again to prevent user select nothing just return
			if (strcmp(m_gpName, "") != 0)
			{
				// get log file name
				strcpy_s(m_logName, sizeof(m_logName), m_gpName);
				// set graph file name
				strcat_s(m_gpName, sizeof(m_gpName), ".txt");
				// set log file name
				strcat_s(m_logName, sizeof(m_logName), "_log.txt");

				fn_DestroyGraph();
				fn_LoadGraph();
				fn_initStrategy();

				if (m_logGen)
					fn_setLogFile();
			}
			// end if if (strcmp(m_gpName, "") != 0)
		}
	}
	else
	{
		m_btnOpen.fn_setbtnState(IDLE);
	}

	// Save button
	if (m_btnSave.fn_isMouseOn(s_MousePosAbs))
	{
		m_btnSave.fn_setbtnState(PASS);

		if (s_isLBtnDown)
		{
			m_btnSave.fn_setbtnState(PUSH);
		}
		else if (s_isLRelease)
		{
			if (!m_Nodes.empty())
			{
				char tmpstr[MAX_PATH] = "";
				strcpy_s(tmpstr, sizeof(tmpstr), openfilename());
				if (strcmp(tmpstr, "") != 0)
				{
					// get log file name
					strcpy_s(m_logName, sizeof(m_logName), tmpstr);
					// set graph file name
					strcat_s(tmpstr, sizeof(tmpstr), ".txt");
					// set log file name
					strcat_s(m_logName, sizeof(m_logName), "_log.txt");
				}
				fn_SaveGraph(tmpstr);
				m_lstBtnGraphLayout.fn_setCurSelecte(0); 
			}
			// end if
		}
	}
	else
	{
		m_btnSave.fn_setbtnState(IDLE);
	}

	// SaveMatrix button
	if (m_btnSaveMatrix.fn_isMouseOn(s_MousePosAbs))
	{
		m_btnSaveMatrix.fn_setbtnState(PASS);

		if (s_isLBtnDown)
		{
			m_btnSaveMatrix.fn_setbtnState(PUSH);
		}
		else if (s_isLRelease)
		{
			if (!m_Nodes.empty())
			{
				char tmpstr[MAX_PATH] = "";
				strcpy_s(tmpstr, sizeof(tmpstr), openfilename());
				if (strcmp(tmpstr, "") != 0)
				{
					// get log file name
					strcpy_s(m_logName, sizeof(m_logName), tmpstr);
					// set graph file name
					strcat_s(tmpstr, sizeof(tmpstr), ".txt");
					// set log file name
					strcat_s(m_logName, sizeof(m_logName), "_log.txt");
				}
				fn_SaveAdjMatrix(tmpstr);
				m_lstBtnGraphLayout.fn_setCurSelecte(0); 
			}
			// end if
		}
	}
	else
	{
		m_btnSaveMatrix.fn_setbtnState(IDLE);
	}
	// end buttons ////

	// list buttons ////
	if (m_lstBtnGraphLayout.fn_isMouseOn(s_MousePosAbs))
	{
		m_lstBtnGraphLayout.fn_setbtnState(PASS);

		if (s_isLClick)
		{
			s_isLClick = false;

			m_lstBtnGraphLayout.fn_select(s_MousePosAbs);
			if (m_lstBtnGraphLayout.fn_getCurrentSelected() == 0)
			{
			}
			else if (m_lstBtnGraphLayout.fn_getCurrentSelected() == 1)
			{
				fn_InitNodePos(L_CIRCLE);
				m_inAnimation = true;
			}
			else if (m_lstBtnGraphLayout.fn_getCurrentSelected() == 2)
			{
				fn_InitNodePos(L_SQUARE);
				m_inAnimation = true;
			}
			// end if else
		}
		// end if (s_isLClick)
	}
	else
	{
		m_lstBtnGraphLayout.fn_setbtnState(IDLE);
	}
	// end list buttons ////

	return false;
}

void Li_GraphPlayer::fn_EditGraph()
{
	static bool lbtnd = false;
	static int lbcounter = 0; // count how many times left btn down

	if (s_isLBtnDown && !lbtnd)
	{
		lbtnd = true;

		  //////////////////
		 // Add New Node //
		//////////////////
		// if L shift is pushed, add an new node
		if (m_DxInput->m_KeyState[DIK_LSHIFT] & 0x80)
		{
			// only add if the mouse click on white space
			bool isSpace = true;
			// test if is Space
			for (int i = (int)m_Nodes.size() - 1; i >= 0; i--)
			{
				float nodeR = m_nodeSprite.fn_getWidth() / 2.0f;
				D3DXVECTOR2 tmpVec = D3DXVECTOR2(0,0);
				// clip nodes
				tmpVec = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_PosX, m_Nodes[i]->m_PosY));

				if (((s_MousePosAbs.x - tmpVec.x)*(s_MousePosAbs.x - tmpVec.x) +
					(s_MousePosAbs.y - tmpVec.y)*(s_MousePosAbs.y - tmpVec.y) <= nodeR*nodeR)) // click right on the node
				{
					isSpace = false;

					// only one can been selected at a time, deal with overlap nodes
					break;
				}
				// end if
			}
			// end for

			if (isSpace)
			{
				D3DXVECTOR2 tmpMousePos = D3DXVECTOR2(0,0);
				// clip mousePos
				tmpMousePos = m_CamMan->fn_getAbsPos(D3DXVECTOR2(s_MousePosAbs.x, s_MousePosAbs.y));

				Li_Node* tmpNode = new Li_Node(m_GraphSize);
				tmpNode->m_PosX = tmpMousePos.x;
				tmpNode->m_PosY = tmpMousePos.y;
				m_Nodes.push_back(tmpNode);
				m_GraphSize++;

				// calculate the graph attribute
				fn_calGraphAttributes();
			}
			// end if
		}
		// end Add New Node ////

		  //////////////////
		 // Add New Edge //
		//////////////////
		else if (m_DxInput->m_KeyState[DIK_LCONTROL] & 0x80)
		{
			// get the node first
			int tmpNode = -1; // -1 means none node selected
			for (int i = (int)m_Nodes.size() - 1; i >= 0; i--)
			{
				float nodeR = m_nodeSprite.fn_getWidth() / 2.0f;
				D3DXVECTOR2 tmpVec = D3DXVECTOR2(0,0);
				// clip nodes
				tmpVec = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_PosX, m_Nodes[i]->m_PosY));

				if (((s_MousePosAbs.x - tmpVec.x)*(s_MousePosAbs.x - tmpVec.x) +
					(s_MousePosAbs.y - tmpVec.y)*(s_MousePosAbs.y - tmpVec.y) <= nodeR*nodeR)) // click right on the node
				{
					tmpNode = i;

					// only one can been selected at a time, deal with overlap nodes
					break;
				}
				// end if
			}
			// end for

			// if there are node selected
			if (tmpNode >= 0)
			{
				static int startV, endV;
				// counter = 0, the start vertex of the edge
				if (lbcounter == 0)
				{
					startV = tmpNode;
					lbcounter = 1;
				}
				else
				{
					// if not the same Node, and the nodes are not already linked already
					if (tmpNode != startV && !fn_Islinked(m_Nodes[startV], m_Nodes[tmpNode]))
					{
						// link the nodes
						endV = tmpNode;
						m_Nodes[startV]->m_Conn.push_back(m_Nodes[endV]);
						m_Nodes[endV]->m_Conn.push_back(m_Nodes[startV]);
						//Li_Edge *tmpEdge = new Li_Edge(startV, endV);
						//m_Edges.push_back(tmpEdge);

						// calculate the graph attribute
						fn_calGraphAttributes();
					}
					lbcounter = 0;
				}
				// end if
			}
			// end if
		}
		// end Add New Edge ////
		
		  /////////////////
		 // Remove Edge //
		/////////////////
		else if(m_DxInput->m_KeyState[DIK_LMENU] & 0x80) // left alt
		{
			// get the node first
			int tmpNode = -1; // -1 means none node selected
			for (int i = (int)m_Nodes.size() - 1; i >= 0; i--)
			{
				float nodeR = m_nodeSprite.fn_getWidth() / 2.0f;
				D3DXVECTOR2 tmpVec = D3DXVECTOR2(0,0);
				// clip nodes
				tmpVec = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_PosX, m_Nodes[i]->m_PosY));

				if (((s_MousePosAbs.x - tmpVec.x)*(s_MousePosAbs.x - tmpVec.x) +
					(s_MousePosAbs.y - tmpVec.y)*(s_MousePosAbs.y - tmpVec.y) <= nodeR*nodeR)) // click right on the node
				{
					tmpNode = i;

					// only one can been selected at a time, deal with overlap nodes
					break;
				}
				// end if
			}
			// end for

			// if there are node selected
			if (tmpNode >= 0)
			{
				static int startV, endV;
				// counter = 0, the start vertex of the edge
				if (lbcounter == 0)
				{
					startV = tmpNode;
					lbcounter = 1;
				}
				else
				{
					// if not the same Node, and the nodes are already linked already
					if (tmpNode != startV && fn_Islinked(m_Nodes[startV], m_Nodes[tmpNode]))
					{
						// link the nodes
						endV = tmpNode;
						int pConn1 = -1, pConn2 = -1;
						for (unsigned int i = 0; i < m_Nodes[startV]->m_Conn.size(); i++)
						{
							if (m_Nodes[startV]->m_Conn[i]->m_ID == endV)
								pConn1 = i;
						}
						
						for (unsigned int i = 0; i < m_Nodes[endV]->m_Conn.size(); i++)
						{
							if (m_Nodes[endV]->m_Conn[i]->m_ID == startV)
								pConn2 = i;
						}
						
						// remove the link
						if (pConn1 >= 0)
							m_Nodes[startV]->m_Conn.erase(m_Nodes[startV]->m_Conn.begin()+pConn1);
						if (pConn2 >= 0)
							m_Nodes[endV]->m_Conn.erase(m_Nodes[endV]->m_Conn.begin()+pConn2);

						// calculate the graph attribute
						fn_calGraphAttributes();
					}
					lbcounter = 0;
				}
				// end if
			}
			// end if
		}
		// end Remove Edge ////
	}
	// mouse left up
	else if (!s_isLBtnDown && lbtnd)
	{
		lbtnd = false;
	}
	// end if
		
	  /////////////////
	 // Remove Node //
	/////////////////
	static bool keycounter = 0;
	if((m_DxInput->m_KeyState[DIK_DELETE] & 0x80) && (keycounter == 0))
	{
		keycounter = 1;

		// get the node first
		int tmpNode = -1; // -1 means none node selected
		for (int i = (int)m_Nodes.size() - 1; i >= 0; i--)
		{
			float nodeR = m_nodeSprite.fn_getWidth() / 2.0f;
			D3DXVECTOR2 tmpVec = D3DXVECTOR2(0,0);
			// clip nodes
			tmpVec = m_CamMan->fn_getRelPos(D3DXVECTOR2(m_Nodes[i]->m_PosX, m_Nodes[i]->m_PosY));

			if (((s_MousePosAbs.x - tmpVec.x)*(s_MousePosAbs.x - tmpVec.x) +
				(s_MousePosAbs.y - tmpVec.y)*(s_MousePosAbs.y - tmpVec.y) <= nodeR*nodeR)) // click right on the node
			{
				tmpNode = i;

				// only one can been selected at a time, deal with overlap nodes
				break;
			}
			// end if
		}
		// end for

		// if there are node selected
		if (tmpNode >= 0)
		{
			while (!m_Nodes[tmpNode]->m_Conn.empty())
			{
				int tmpConn = m_Nodes[tmpNode]->m_Conn.back()->m_ID;
				int pConn1 = -1, pConn2 = -1;
				for (unsigned int i = 0; i < m_Nodes[tmpNode]->m_Conn.size(); i++)
				{
					if (m_Nodes[tmpNode]->m_Conn[i]->m_ID == tmpConn)
						pConn1 = i;
				}
						
				for (unsigned int i = 0; i < m_Nodes[tmpConn]->m_Conn.size(); i++)
				{
					if (m_Nodes[tmpConn]->m_Conn[i]->m_ID == tmpNode)
						pConn2 = i;
				}
						
				// remove the link
				if (pConn1 >= 0)
					m_Nodes[tmpNode]->m_Conn.erase(m_Nodes[tmpNode]->m_Conn.begin()+pConn1);
				if (pConn2 >= 0)
					m_Nodes[tmpConn]->m_Conn.erase(m_Nodes[tmpConn]->m_Conn.begin()+pConn2);
			}
			// end while

			m_Nodes.erase(m_Nodes.begin()+tmpNode);

			// calculate the graph attribute
			fn_calGraphAttributes();

			fn_UpdateFrame();
		}
		// end if
	}
	else if (!(m_DxInput->m_KeyState[DIK_DELETE] & 0x80) && (keycounter != 0))
	{
		keycounter = 0;
	}
	// end Remove Node ////
}

void Li_GraphPlayer::fn_log()
{
	// columm approach
	// m_fout<<m_GenNum<<".\t"<<m_CR<<endl;
	
	// row approach
	if (m_GenNum == 0)
		m_fout<<s_expNum<<".\t"<<m_CR;
	else if (m_GenNum == s_maxGen)
		m_fout<<"\t"<<m_CR<<endl;
	else
		m_fout<<"\t"<<m_CR;
}