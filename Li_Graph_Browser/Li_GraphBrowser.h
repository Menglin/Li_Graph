#ifndef LI_GRAPH_BROWSER_H
#define LI_GRAPH_BROWSER_H

#include "../Li_Graphic_Framework/Li_UTI_FrameWork.h"
#include "../Li_Graph/Li_Graph.h"
#pragma comment(lib, "../debug/Li_Graphic_FrameWork.lib")
#pragma comment(lib, "../debug/Li_Graph.lib")

class Li_GraphBrowser : public Li_Graph, public Li_FW
{
public:
	virtual void fn_DrawGraph();

	virtual HRESULT fn_ctrl_initD3D();
	virtual HRESULT fn_ctrl_releaseD3D();
	virtual void fn_ctrl_mainLogic();
	virtual void fn_ctrl_d3dRender();
	virtual void fn_keyListener();
	virtual void fn_mouseListener();

protected:
	Li_Sprite m_nodeSprite;
	Li_Sprite m_textBoxSprite;
};

static Li_GraphBrowser *mygb;
#endif