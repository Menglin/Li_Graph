#ifndef LI_GRAPH_PLAYER_H
#define LI_GRAPH_PLAYER_H

#include "../Li_Graph_Browser/Li_GraphBrowser.h"
#include "../Li_Graphic_FrameWork/Li_UTI_UI.h"

#include "Li_Game.h"
#include <time.h>
enum ENUM_STATE
{
	INIT,
	RUNNING,
	PAUSE
};

class Li_GraphPlayer : public Li_GraphBrowser, public Li_PDGame
{
public:
	Li_GraphPlayer()
	{
		m_state = PAUSE;
	}
	virtual void fn_DrawGraph();

	virtual HRESULT fn_ctrl_initD3D();
	virtual HRESULT fn_ctrl_releaseD3D();
	virtual void fn_ctrl_mainLogic();
	virtual void fn_ctrl_d3dRender();

	void fn_goEpic();
	void fn_calPayoff();
	void fn_calCR();
	void fn_updateGen();

	void fn_initStrategy();

	virtual void fn_keyListener();
	virtual void fn_mouseListener();

	void fn_drawUI();

protected :
	time_t m_LastUpdateTime;
	int m_GenNum;
	double m_CR;

	ENUM_STATE m_state;

	Li_btnSprite m_btnRun;
	Li_btnSprite m_btnPause;
	Li_btnSprite m_btnStop;
};

static Li_GraphPlayer *mygp;
#endif