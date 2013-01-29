#include <fstream>
#include <math.h>
#include "Li_Graph.h"
#include "../Li_ComnHeader/Li_MathConst.h"

  /*----------------
 // Common Graph //
----------------*/
// Create inputed numbers Nodes without edge
inline void Li_Graph::fn_CreateNodes(int numNodes)
	{
		if (numNodes > 0)
		{
			Li_Node* tmpNode	= NULL;
			for (int i = 0; i < numNodes; i++)
			{
				tmpNode = new Li_Node(i);
				m_Nodes.push_back(tmpNode);
				tmpNode = NULL;
			}
			// end for
		}
		// end if
	}

// check if two nodes in the graph is linked already
bool Li_Graph::fn_Islinked(Li_Node* N1, Li_Node* N2)
{
	bool linked = false;

	for (unsigned int i = 0; i < N1->m_Conn.size(); i++)
	{
		if (N2 == N1->m_Conn[i])
		{
			linked = true;
			break;
		}
	} // end for i

	return linked;
}

// Create certain size of Graph with expected Clustering Coefficient within expected Error
double Li_Graph::fn_GenerateGraph(int expSize, double expCC, double expError)
{
	// make sure the graph have more than three nodes
	if (expSize < 3)
		return LESS_THAN_THREE_NODE;

	double actError		= 0;
	m_GraphSize			= 0;
	m_Edges				= 0;
	m_Triplets			= 0;
	m_Triangles			= 0;
	Li_Node* tmpNode	= NULL;
	m_TotalCC					= 0.0;

	// Step 1. Create a non-triangle. connective Graph with the expected Size
	// // 1-1. setup first 3s nodes, and connect them together
	for (int i = 0; i < 3; i++)
	{
		tmpNode = new Li_Node(i);		// Build an nodes and input nod id
		m_Nodes.push_back(tmpNode);		// add the Node in to the Nodes vector
		tmpNode = NULL;					// free the pointer here, prevent protential crash later

		// do the connection (1-2-3), link 1-2, and 2-3
		if (i != 0)
		{
			m_Nodes[i-1]->m_Conn.push_back(m_Nodes[i]);
			m_Nodes[i]->m_Conn.push_back(m_Nodes[i-1]);
			// increase the local degree of the two node by one;
			m_Nodes[i-1]->m_LocDegree++;
			m_Nodes[i]->m_LocDegree++;
		}
	}
	// end for

	// now we have 3 nodes, with 2 links, linked as a line, which includes 1 triplets
	m_GraphSize	= 3;
	m_Edges			= 2;
	m_Triplets			= 1;

	// // 1-2. add the rest nodes and randomly link it to one of the exist node
	for (int i = 3; i < expSize; i++)
	{
		tmpNode = new Li_Node(i);			// Build an Node and input nod id
		m_Nodes.push_back(tmpNode);			// add the Node in to the Nodes vector
		tmpNode = NULL;						// free the pointer here, prevent protential crash later

		int tmpLink = rand()%m_GraphSize;	// get a random node from the exists nodes
		m_Nodes[i]->m_Conn.push_back(m_Nodes[tmpLink]);
		m_Nodes[tmpLink]->m_Conn.push_back(m_Nodes[i]);
		// increase the local degree of the two node by one;
		m_Nodes[i]->m_LocDegree++;
		m_Nodes[tmpLink]->m_LocDegree++;

		// increase the graph size and the edges
		m_GraphSize++;
		m_Edges++;
		// increase corresponding triplets
		m_Triplets += (m_Nodes[tmpLink]->m_Conn.size() - 1);
	}
	// end for
	// after Step 1. the local clustering coefficient of each node in the graph should be all 0

	// Step 2. add triangles to increase the clustering coefficient
	// deal with 0 clustering coefficient.
	if (expCC == 0.0)
		return 0;
	// *Also Step 2. loop until the actual total clustering coefficient liner approximate the expected clustering coefficient within the expected error
	// always do first, because our privous setting can ensure there are no clustering coeffecit at all.
	do
	{
		// random pick an entrance
		int tmpEntrance = rand()%m_GraphSize;

		// find the first available Node who can used to add triangle from the entrance
		bool linkable = false;

		int tmpNum = 0;	// used to mark the node that we random selected to create the triangle
		int tmpC1 = 0;	// used to mark the first conn of the node to create the new connection
		int tmpC2 = 0;	// used to mark the second conn of the node to create the new connection

		// // 2-1. add triangle edge
		// // // 2-1-1. looking for the first valid triangle link start from a random entrance
		// randomly pick an entrance, and if it is not valid, then move to the immidiately next, and the search will end once reached the entire loop
		unsigned int i = 0;
		while (	i < m_Nodes.size() && !linkable)	// loop for each Nodes, no more
		{
			// get the current Node
			unsigned int tmpCurrent = tmpEntrance + i;
			if (tmpCurrent >= m_Nodes.size())
				tmpCurrent -= m_Nodes.size();

			// if the current Node have more than one connection and Local Cluster Coefficient < 1
			if (m_Nodes[tmpCurrent]->m_Conn.size() > 1
				&& m_Nodes[tmpCurrent]->m_LocCC < 1.0)
			{
				// looking for the linkable pair
				int tmpRand1 = rand()%m_Nodes[tmpCurrent]->m_Conn.size();
				int tmpRand2 = rand()%m_Nodes[tmpCurrent]->m_Conn.size();
				while (tmpRand2 == tmpRand1)
					tmpRand2 = rand()%m_Nodes[tmpCurrent]->m_Conn.size();

				// now we got 2 different Node, test if they are linkable
				// start from the random Node 1, if it not possible, then move to the next
				unsigned int j = 0;
				while (j < m_Nodes[tmpCurrent]->m_Conn.size() && !linkable)
				{
					unsigned int tmpNode1 = tmpRand1 + j;
					if (tmpNode1 >= m_Nodes[tmpCurrent]->m_Conn.size())
						tmpNode1 -= m_Nodes[tmpCurrent]->m_Conn.size();

					// start from the random Node 2, if it not possible, then move to the next
					unsigned int k = 0;
					while (k < m_Nodes[tmpCurrent]->m_Conn.size() && !linkable)
					{
						unsigned int tmpNode2 = tmpRand2 + k;
						if (tmpNode2 >= m_Nodes[tmpCurrent]->m_Conn.size())
							tmpNode2 -= m_Nodes[tmpCurrent]->m_Conn.size();

						// if A1and A2 do not linked and they are not the same Node
						if (!fn_Islinked(m_Nodes[tmpCurrent]->m_Conn[tmpNode1], m_Nodes[tmpCurrent]->m_Conn[tmpNode2]) && tmpNode1 != tmpNode2)
						{
							linkable = true;
							// get the link number
							tmpNum = tmpCurrent;
							tmpC1 = tmpNode1;
							tmpC2 = tmpNode2;
						}
						// end if
						k++;
					}
					// end while k
					j++;
				}
				// end while j
			}
			// end if
			i++;
		}
		// end while i

		// // // 2-1-2. add the link based on the previous finding
		// if linkable, link them if it is an valid link, add a triangle
		if (linkable)
		{
			// link the two neighbours to each other
			m_Nodes[tmpNum]->m_Conn[tmpC1]->m_Conn.push_back(m_Nodes[tmpNum]->m_Conn[tmpC2]);
			m_Nodes[tmpNum]->m_Conn[tmpC2]->m_Conn.push_back(m_Nodes[tmpNum]->m_Conn[tmpC1]);
			// increase the local degree of the two node by one;
			m_Nodes[tmpNum]->m_Conn[tmpC1]->m_LocDegree++;
			m_Nodes[tmpNum]->m_Conn[tmpC2]->m_LocDegree++;

			// // 2-2. if the edge are added, calculate the new triplets and triangles in the new graph
			// // // 2-2-1. count how many neighbours the two linked node have, it is the number of the new addon triplets
			// update the triplets (the new addon triplets is the sum of the two node's degree, because they already linked to each other, so, minus 1 on each degree)
			m_Triplets += (m_Nodes[tmpNum]->m_Conn[tmpC1]->m_Conn.size() - 1 + m_Nodes[tmpNum]->m_Conn[tmpC2]->m_Conn.size() - 1);
			// // // 2-2-2. the number of shared neighbour of these two nodes times 3 is the number of the triangles
			int tmpTriangle = 0;
			for (unsigned int i = 0; i < m_Nodes[tmpNum]->m_Conn[tmpC1]->m_Conn.size(); i++)
			{
				for (unsigned int j = 0; j < m_Nodes[tmpNum]->m_Conn[tmpC2]->m_Conn.size(); j++)
				{
					if (m_Nodes[tmpNum]->m_Conn[tmpC1]->m_Conn[i] == m_Nodes[tmpNum]->m_Conn[tmpC2]->m_Conn[j])
					{
						tmpTriangle++;
						// // // // update the local clustering coefficient for the updated nodes. // // // //
						// update the number of the connections of the Node's connection cluster
						// realize the selected Node is not the only Node that influnced by the adding edge,
						// all the Nodes that connected to the two newly linked Nodes have to update their local clustering coefficient
						// so we have to go through all the nodes that may affected to find out which one needs to be update.
						// *1. add 1 to it cluster's connection
						m_Nodes[tmpNum]->m_Conn[tmpC1]->m_Conn[i]->m_SubGraphConn++;
						// *2. compute the new local clustering coefficient
						int tmpDegree = m_Nodes[tmpNum]->m_Conn[tmpC1]->m_Conn[i]->m_Conn.size();
						int tmpMaxConn = tmpDegree * (tmpDegree - 1) / 2;
						int tmpCurConn = m_Nodes[tmpNum]->m_Conn[tmpC1]->m_Conn[i]->m_SubGraphConn;
						m_Nodes[tmpNum]->m_Conn[tmpC1]->m_Conn[i]->m_LocCC =  tmpCurConn / tmpMaxConn;
					}
				}
				// end for j
			}
			// end for i

			// update the Triangles
			m_Triangles += tmpTriangle*3;
			// // // 2-2-3. calculate the Total Clustering coefficient
			// compute the total clustering coefficient
			m_TotalCC = double(m_Triangles) / double(m_Triplets);

			// // // // Calculate others attribute // // // //
			// These attribute is not necessary for the graph generate, but it is much easier(less time complexity) to compute while building the graph, and may have good use in the furture
		}
		// end if (linkable)

		// // 2-3. calculate the errors, do not use the abs, because it may jumped over the error set
		// however, if it already greater than the expected value, adding triangles will only make it even greater
		actError = expCC - m_TotalCC;
	} while (actError > expError && m_TotalCC < 1.0);

	// // // // calculate the average degree and average clustering coefficient // // // //

	return actError;
}

// Create certain size of Graph with expected Clustering Coefficient within expected Error, and expected Degree
//double fnGenerateGraph(int expSize, double expCC, double expError, int expDegree);

// calculate graph attributes
void Li_Graph::fn_calGraphAttributes()
{
	int		sumDegree	= 0;	// used to calculate the average graph degree
	double	sumLocCC	= 0;		// used to calculate the average graph Clustering coefficient
	// initialize the graph attribute calculate sensitive data for the entire graph
	m_GraphSize	= (int)m_Nodes.size();
	m_Edges		= 0;
	m_AvgDegree	= 0;
	m_AvgCC		= 0.0;
	m_TotalCC	= 0.0;

	m_Triplets	= 0;
	m_Triangles	= 0;

	// 1. go through every node
	for (int i = 0; i < (int)m_Nodes.size(); i++)
	{
		// initialize the graph attribute calculate sensitive data for the node
		m_Nodes[i]->m_LocDegree		= m_Nodes[i]->m_Conn.size();
		m_Nodes[i]->m_LocCC			= 0.0;
		m_Nodes[i]->m_SubGraphConn	= 0;

		// 2. see if this guy could be a top node of triangle or triplets (if its has more than 2 connection)
		if (m_Nodes[i]->m_Conn.size() >= 2)
		{
			// for each node who is a top node
			// 2.1. calculate the protencial sub Graph connections
			int maxSubGraphConn = (m_Nodes[i]->m_LocDegree * (m_Nodes[i]->m_LocDegree-1)) / 2;
			// 2.2. the number of the triplets by this node increased by the number of protencial sub graph connections
			m_Triplets += maxSubGraphConn;

			// 2.3. go through all of his neighbour
			for (int j = 0; j < (int)m_Nodes[i]->m_Conn.size(); j++)
			{
				// check if this neighbour is connected to its other neighbour
				for (int k = j; k < (int)m_Nodes[i]->m_Conn.size(); k++)
				{
					if (fn_Islinked(m_Nodes[i]->m_Conn[j], m_Nodes[i]->m_Conn[k]))
					{
						// for local cc, increase the actrul subGraph Conn
						m_Nodes[i]->m_SubGraphConn++;

						// for total cc, increase the Triangle numbers by 1
						// here I did not increase it by three, is because I only count the triangle once with current node as main top
						// when we count other two top node as main top, we will re count it, this is differ from when we create the graph
						// also, we could see the different of locCC, avgCC and totalCC from here
						m_Triangles++;
					}
					// end if
				}
				// end for k
			}
			// end for j

			// once I finished check all connections from one node, we could calculate it's local CC here
			// don't worry about the if, if it less than 2 node, its localCC can only be 0
			m_Nodes[i]->m_LocCC = (double)m_Nodes[i]->m_SubGraphConn / maxSubGraphConn;
			sumLocCC += m_Nodes[i]->m_LocCC;
		}
		// end  if (m_Nodes[i]->m_Conn.size() >= 2)

		// increase the sumDegree;
		sumDegree += m_Nodes[i]->m_LocDegree;
	}
	// end for i

	// 3. after traversd the nodes in the graph
	// 3.1. get the number of edges
	// the number of edges is the sumDegree/2
	m_Edges		= sumDegree / 2;
	// 3.2. get the average degree
	m_AvgDegree	= (double)sumDegree / m_GraphSize;
	// 3.3. get the average CC
	m_AvgCC		= sumLocCC / m_GraphSize;
	// 3.4. get the total CC
	m_TotalCC	= (double)m_Triangles / m_Triplets;
}

// save the Graph to a file
bool Li_Graph::fn_SaveGraph(char* filename)
{
	ofstream fout;
	fout.open(filename, ios::out);

	fout<<m_Nodes.size()<<"\n";
	for (unsigned int i = 0; i < m_Nodes.size(); i++)
	{
		fout<<m_Nodes[i]->m_ID<<"\t"<<m_Nodes[i]->m_Conn.size()<<"\t{\t";
		for (unsigned int j = 0; j < m_Nodes[i]->m_Conn.size(); j++)
		{
			fout<<m_Nodes[i]->m_Conn[j]->m_ID<<"\t";
		}
		fout<<"};\n";
	}

	fout.close();
	return true;
}

// load the Graph from a file
bool Li_Graph::fn_LoadGraph(char* filename)
{
	ifstream fin;
	fin.open(filename, ios::in);

	if (!fin.is_open())
		return false;

	int numNodes, curNode, numConn, connNode;
	char c; // temp save the characters
	//int pointerpos; // for test

	// get the total number of nodes of the graph first
	fin>>numNodes;
	// create only nodes for the graph
	fn_CreateNodes(numNodes);

	// get the first current node,
	fin>>curNode;
	// get the node connection
	while(fin.good())
	{
		// get the number of its connection, and a '{' character
		fin>>numConn>>c;
		// create the link
		for (int i = 0; i < numConn; i++)
		{
			// get the connection destination
			fin >>connNode;
			m_Nodes[curNode]->m_Conn.push_back(m_Nodes[connNode]);
		}
		// looking for the line end character ';'
		do
			fin>>c;
		while(c!=';');
		// get the next current node,
		// always get the next current node in the previous loop
		// to make sure the loop know the next line is empty,
		// prevent the last connection being connected twice
		fin>>curNode;
		//pointerpos = fin.tellg(); // for test
	}

	fin.close();

	// calculate the graph attributes
	fn_calGraphAttributes();

	return true;
}

void Li_Graph::fn_InitNodePos()
{
	int num = m_Nodes.size();

	int radius = num * 10;
	if (radius < 50)
		radius = 50;
	else if (radius > 200)
		radius = 200;

	int centerx = radius + 50;
	int centery = radius + 50;

	int numPerC = num;
	if (numPerC > 200)
		numPerC = 200;

	bool screw = false;
	if (num > numPerC)
		screw = true;

	for (int i = 0; i < num; i++)
	{
		m_Nodes[i]->m_PosX = centerx + radius *(float)cos(2 * PI * i / numPerC);
		m_Nodes[i]->m_PosY = centery + radius *(float)sin(2 * PI * i / numPerC);
		if (screw)
			radius++;
		// end if
	}
	// end for
}