#ifndef LI_GRAPH_PLAYER_H
#define LI_GRAPH_PLAYER_H

#include "../Li_Graph_Browser/Li_GraphBrowser.h"

#include "Li_Game.h"
#include <time.h>

class Li_GraphPlayer : public Li_GraphBrowser, public Li_PDGame
{
public:
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

protected :
	time_t m_LastUpdateTime;
	int m_GenNum;
	double m_CR;
};

static Li_GraphPlayer *mygp;
#endif