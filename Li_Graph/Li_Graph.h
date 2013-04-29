#ifndef LI_GRAPH_H
#define LI_GRAPH_H

#include "Li_Node.h"
#include "Li_Edge.h"

#define LESS_THAN_THREE_NODE -9999 // an Error feed back of the function return value

enum GraphLayout
{
	L_CIRCLE,
	L_SQUARE,
	L_FILE
};

  /*----------------
 // Common Graph //
----------------*/
class Li_Graph
{
public:
	Li_Graph()
	{
		fn_InitGraph();
		strcpy_s(m_gpName, sizeof(m_gpName), "");
		strcpy_s(m_gpName, sizeof(m_gpName), "");
	}

	~Li_Graph()
	{
		fn_DestroyGraph();
	}

	virtual void fn_InitGraph()
	{
		m_GraphSize		= 0;
		m_EdgeNum		= 0;
		m_AvgDegree		= 0;
		m_AvgCC			= 0.0;
		m_Transitivity	= 0.0;

		m_Triplets		= 0;
		m_Triangles		= 0;
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

		/*
		while (!m_Edges.empty())
		{
			delete m_Edges.back();
			m_Edges.pop_back();
		}
		*/

		m_GraphSize		= 0;
		m_EdgeNum		= 0;
		m_AvgDegree		= 0;
		m_AvgCC			= 0.0;
		m_Transitivity	= 0.0;

		m_Triplets		= 0;
		m_Triangles		= 0;
	}

	virtual void fn_ReInitGraph()
	{
		fn_DestroyGraph();
		fn_InitGraph();
	}

	void fn_setGraphName(char* name)
	{
		strcpy_s(m_gpName, sizeof(m_gpName), name);
	}

	// Create inputed numbers Nodes without edge
	inline void fn_CreateNodes(int numNodes = 0);

	// check if two nodes in the graph is linked already
	bool fn_Islinked(Li_Node* N1, Li_Node* N2); 

	// Create certain size of Graph with expected Clustering Coefficient within expected Error
	double fn_GenerateGraphASC(int expSize, double expCC, double expError, bool displayProcess = false);
	double fn_GenerateGraphASCGroup(int expSize, double expCC, double expError, bool displayProcess = false);
	//double fn_GenerateGraphDESC(int expSize, double expCC, double expError, bool displayProcess = false);
	// Create certain size of Graph with expected Clustering Coefficient within expected Error, and expected Degree
	double fn_GenerateGraph(int expSize, double expCC, double expError, int expDegree, bool displayProcess = false);
	
	// generate graph genetic
	//void fn_GenerateGraphGA(int expSize);

	// calculate graph attributes
	void fn_calGraphAttributes();

	// save the Graph to a file
	bool fn_SaveGraph(char* filename = "");
	// save the Graph to a adjcency matrix
	bool fn_SaveAdjMatrix(char* filename = "");
	// load the Graph from a file
	bool fn_LoadGraph(char* filename = "");
	// Initialize the position of Nodes as a circle
	void fn_InitNodePos(GraphLayout layout);

//protected:
	vector<Li_Node*> m_Nodes;
	//vector<Li_Edge*> m_Edges; // for undirected graph, this is not really helpful, only adding redanduncy

	int		m_GraphSize;	// the number of nodes in the graph
	int		m_EdgeNum;		// the total edges in the graph
	double	m_AvgDegree;	// the average degree of the graph
	double	m_AvgCC;		// the average cluster coefficient of the graph
	double	m_Transitivity;	// the Total cluster coefficient of the graph

	// to reduce the time of calculate the total clustering coefficient
	int		m_Triplets;		// the number of triplets in the graph
	int		m_Triangles;	// the number of triangles in the graph

	char	m_gpName[260];	// used to mark the graph and log file's name
	char	m_logName[260];	// used to mark the graph and log file's name
};

#endif