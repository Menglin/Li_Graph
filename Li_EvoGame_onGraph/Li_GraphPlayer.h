#ifndef LI_GRAPH_PLAYER_H
#define LI_GRAPH_PLAYER_H

#include "../Li_Graphic_Framework/Li_UTI_FrameWork.h"
#include "../Li_Graph/Li_Graph.h"
#include "../Li_Graphic_FrameWork/Li_UTI_UI.h"

#include "Li_Game.h"
#include <time.h>

#pragma comment(lib, "../debug/Li_Graphic_FrameWork.lib")
#pragma comment(lib, "../debug/Li_Graph.lib")

static const DWORD g_UpdateTime = 200; // in millisecond

enum ENUM_STATE
{
	INIT,
	RUNNING,
	PAUSE
};

class Li_GraphPlayer : public Li_Graph, public Li_FW, public Li_PDGame
{
public:
	Li_GraphPlayer()
	{
		m_state = PAUSE;
		m_displayGraph = true;
		m_logGen = true;

		// the log file will be opend if the file name has been assigned
		/*
		m_fout.open("log_cr.txt", ios::app);

		if (!m_fout.is_open())
			m_fout.open("log_cr.txt", ios::out);
		*/
	}

	~Li_GraphPlayer()
	{
		if (m_fout != NULL)
			m_fout.close();
	}

	virtual void fn_Play(Li_Node *a, Li_Node *b, bool isDoubleDir){Li_PDGame::fn_Play(a,b,isDoubleDir);}
	virtual void fn_Evolve(Li_Node *a){Li_PDGame::fn_Evolve(a);}

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

	virtual bool fn_keyListener();
	virtual bool fn_mouseListener();

	void fn_drawUI();

	void fn_log();
	void fn_setLogFile()
	{
		// if already opened it, close it first
		// to prevent the new logfile not be opened when the user change the graph file
		if (m_fout != NULL)
			m_fout.close();
		// the log file will be opend if the file name has been assigned
		m_fout.open(m_logName, ios::app);

		if (!m_fout.is_open())
			m_fout.open(m_logName, ios::out);
	}

protected :
	Li_Sprite m_nodeSprite;
	Li_Sprite m_textBoxSprite;

	DWORD m_LastUpdateTime;
	int m_GenNum;
	double m_CR;

	ENUM_STATE m_state;

	Li_btnSprite m_btnRun;
	Li_btnSprite m_btnPause;
	Li_btnSprite m_btnStop;
	Li_btnSprite m_btnDisplay;
	Li_btnSprite m_btnLog;
	
	Li_lstBtnSprite m_lstBtnGraphLayout;

	Li_btnSprite m_btnSave;
	Li_btnSprite m_btnSaveMatrix;

	bool m_displayGraph;
	bool m_logGen;

	ofstream m_fout;
};

static Li_GraphPlayer *mygp;
#endif