#ifndef LI_GRAPH_H
#define LI_GRAPH_H

#include "Li_Node.h"

#define LESS_THAN_THREE_NODE -9999 // an Error feed back of the function return value

  /*----------------
 // Common Graph //
----------------*/
class Li_Graph
{
public:
	Li_Graph()
	{
		fn_InitGraph();
	}

	~Li_Graph()
	{
		fn_DestroyGraph();
	}

	virtual void fn_InitGraph()
	{
		m_GraphSize	= 0;
		m_Edges		= 0;
		m_AvgDegree	= 0;
		m_AvgCC		= 0.0;
		m_TotalCC	= 0.0;

		m_Triplets	= 0;
		m_Triangles	= 0;
	}

	virtual void fn_DestroyGraph()
	{
		while (!m_Nodes.empty())
		{
			delete m_Nodes.back();
			m_Nodes.pop_back();
		}
		/*
		for (unsigned int i = 0; i < m_Nodes.size(); i++)
			delete(m_Nodes[i]);

		m_Nodes.clear();
		*/

		m_GraphSize	= 0;
		m_Edges		= 0;
		m_AvgDegree	= 0;
		m_AvgCC		= 0.0;
		m_TotalCC	= 0.0;

		m_Triplets	= 0;
		m_Triangles	= 0;
	}

	virtual void fn_ReInitGraph()
	{
		fn_DestroyGraph();
		fn_InitGraph();
	}

	// Create inputed numbers Nodes without edge
	inline void fn_CreateNodes(int numNodes = 0);

	// check if two nodes in the graph is linked already
	bool fn_Islinked(Li_Node* N1, Li_Node* N2); 

	// Create certain size of Graph with expected Clustering Coefficient within expected Error
	double fn_GenerateGraph(int expSize, double expCC, double expError);
	// Create certain size of Graph with expected Clustering Coefficient within expected Error, and expected Degree
	double fn_GenerateGraph(int expSize, double expCC, double expError, int expDegree);

	// calculate graph attributes
	void fn_calGraphAttributes();

	// save the Graph to a file
	bool fn_SaveGraph(char* filename);
	// load the Graph from a file
	bool fn_LoadGraph(char* filename);
	// Initialize the position of Nodes as a circle
	void fn_InitNodePos();

//protected:
	vector<Li_Node*> m_Nodes;

	int		m_GraphSize;	// the number of nodes in the graph
	int		m_Edges;		// the total edges in the graph
	double	m_AvgDegree;	// the average degree of the graph
	double	m_AvgCC;		// the average cluster coefficient of the graph
	double	m_TotalCC;		// the Total cluster coefficient of the graph

	// to reduce the time of calculate the total clustering coefficient
	int		m_Triplets;		// the number of triplets in the graph
	int		m_Triangles;	// the number of triangles in the graph
};

#endif