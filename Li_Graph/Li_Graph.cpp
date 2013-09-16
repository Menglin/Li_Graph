#include <fstream>
#include <string>
#include <math.h>
#include "Li_Graph.h"
#include "../Li_ComnHeader/Li_MathConst.h"
#include "../Li_ComnHeader/Li_ComnFunc.h"

#include <iostream>

// Nodes Layout

#define NUMPERC 200

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
double Li_Graph::fn_GenerateGraphASC(int expSize, double expCC, double expError, bool displayProcess)
{
	// make sure the graph have more than three nodes
	if (expSize < 3)
		return LESS_THAN_THREE_NODE;

	double actError		= 0;
	m_GraphSize			= 0;
	m_EdgeNum			= 0;
	m_Triplets			= 0;
	m_Triangles			= 0;
	Li_Node* tmpNode	= NULL;
	m_Transitivity		= 0.0;
	

	// for initialize the position as Circle by default,
	// didn't call the method because I merged the two loop together to save time complecity
	int num = expSize;
	// Circle layout
	int radius = num * 10;
	if (radius < 50)
		radius = 50;
	else if (radius > 200)
		radius = 200;

	int centerx = radius + 100;
	int centery = radius + 100;

	int numPerC = num;
	if (numPerC > NUMPERC)
		numPerC = NUMPERC;

	bool screw = false;
	if (num > numPerC)
		screw = true;

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

		// initialize the position as Circle by default,
		// didn't call the method because I merged the two loop together to save time complecity
		m_Nodes[i]->m_PosX = centerx + radius *(float)cos(2 * PI * i / numPerC);
		m_Nodes[i]->m_PosY = centery + radius *(float)sin(2 * PI * i / numPerC);
		if (screw)
			radius++;
	}
	// end for

	// now we have 3 nodes, with 2 links, linked as a line, which includes 1 triplets
	m_GraphSize	= 3;
	m_EdgeNum	= 2;
	m_Triplets	= 1;

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
		m_EdgeNum++;
		// increase corresponding triplets
		m_Triplets += (m_Nodes[tmpLink]->m_Conn.size() - 1);

		// initialize the position as Circle by default,
		// didn't call the method because I merged the two loop together to save time complecity
		m_Nodes[i]->m_PosX = centerx + radius *(float)cos(2 * PI * i / numPerC);
		m_Nodes[i]->m_PosY = centery + radius *(float)sin(2 * PI * i / numPerC);
		if (screw)
			radius++;
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

						// if A1 and A2 do not linked and they are not the same Node
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

			m_EdgeNum++;

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
			m_Transitivity = double(m_Triangles) / double(m_Triplets);

			// // // // Calculate others attribute // // // //
			// These attribute is not necessary for the graph generate, but it is much easier(less time complexity) to compute while building the graph, and may have good use in the furture
		}
		// end if (linkable)

		// // 2-3. calculate the errors, do not use the abs, because it may jumped over the error set
		// however, if it already greater than the expected value, adding triangles will only make it even greater
		actError = expCC - m_Transitivity;

		if (displayProcess)
			std::cout<<expSize<<", "<<m_EdgeNum<<", "<<m_Triangles<<" / "<<m_Triplets<<" : "<<m_Transitivity<<endl;
	} while (actError > expError && m_EdgeNum < expSize*(expSize-1)/2); // && m_TotalCC < 1.0 && m_TotalCC >= 0);

	// // // // calculate the average degree and average clustering coefficient // // // //

	return actError;
}

double Li_Graph::fn_GenerateGraphASCGroup(int expSize, double expCC, double expError, bool displayProcess)
{
	
	// make sure the graph have more than three nodes
	if (expSize < 3)
		return LESS_THAN_THREE_NODE;

	double actError		= 0;
	m_GraphSize			= 0;
	m_EdgeNum			= 0;
	m_Triplets			= 0;
	m_Triangles			= 0;
	Li_Node* tmpNode	= NULL;
	m_Transitivity			= 0.0;
	
	int lastRecEdges	= 0;	// remember the edge number of the last time record the graph into the file
	int recGap			= 1000;	// The gap between two record (because we record the graph by actError, some times, between the actError and expError, there may have multiple valid graphs)

	// for initialize the position as Circle by default,
	// didn't call the method because I merged the two loop together to save time complecity
	int num = expSize;
	// Circle layout
	int radius = num * 10;
	if (radius < 50)
		radius = 50;
	else if (radius > 200)
		radius = 200;

	int centerx = radius + 100;
	int centery = radius + 100;

	int numPerC = num;
	if (numPerC > NUMPERC)
		numPerC = NUMPERC;

	bool screw = false;
	if (num > numPerC)
		screw = true;

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

		// initialize the position as Circle by default,
		// didn't call the method because I merged the two loop together to save time complecity
		m_Nodes[i]->m_PosX = centerx + radius *(float)cos(2 * PI * i / numPerC);
		m_Nodes[i]->m_PosY = centery + radius *(float)sin(2 * PI * i / numPerC);
		if (screw)
			radius++;
	}
	// end for

	// now we have 3 nodes, with 2 links, linked as a line, which includes 1 triplets
	m_GraphSize	= 3;
	m_EdgeNum	= 2;
	m_Triplets	= 1;

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
		m_EdgeNum++;
		// increase corresponding triplets
		m_Triplets += (m_Nodes[tmpLink]->m_Conn.size() - 1);

		// initialize the position as Circle by default,
		// didn't call the method because I merged the two loop together to save time complecity
		m_Nodes[i]->m_PosX = centerx + radius *(float)cos(2 * PI * i / numPerC);
		m_Nodes[i]->m_PosY = centery + radius *(float)sin(2 * PI * i / numPerC);
		if (screw)
			radius++;
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

						// if A1 and A2 do not linked and they are not the same Node
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

			m_EdgeNum++;

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
			m_Transitivity = double(m_Triangles) / double(m_Triplets);

			// // // // Calculate others attribute // // // //
			// These attribute is not necessary for the graph generate, but it is much easier(less time complexity) to compute while building the graph, and may have good use in the furture
		}
		// end if (linkable)

		// // 2-3. calculate the errors, do not use the abs, because it may jumped over the error set
		// however, if it already greater than the expected value, adding triangles will only make it even greater
		actError = expCC - m_Transitivity;

		if (displayProcess)
		{
			if (m_EdgeNum%1000 == 0)
				std::cout<<expSize<<", "<<m_EdgeNum<<", "<<m_Triangles<<" / "<<m_Triplets<<" : "<<m_Transitivity<<"\r";
		}

		double absError = abs(actError);
		if (absError < expError && (lastRecEdges == 0 || m_EdgeNum - lastRecEdges > recGap))
		{
			char fname[32] = "newgraph";
			char tmpName[16] = "";
			ParseStr(tmpName, m_EdgeNum);
			strcat_s(fname, tmpName);
			strcat_s(fname, ".txt");
			fn_SaveGraph(fname);

			lastRecEdges = m_EdgeNum;

			std::cout<<fname<<", "<<m_EdgeNum<<", "<<m_Triangles<<" / "<<m_Triplets<<" : "<<m_Transitivity<<endl;
		}
	} while (/*actError > expError && */m_EdgeNum < expSize*(expSize-1)/2); // && m_TotalCC < 1.0 && m_TotalCC >= 0);
	// to generate a group of certain cc graph, we dont stop the loop when it reach the experror, we keep looping it,
	// every time it is hit the experror, we output a graph, and then give it sever edges to jump out of the current error range.

	return 0;
}

/*
// Create certain size of Graph with expected Clustering Coefficient within expected Error
double Li_Graph::fn_GenerateGraphDESC(int expSize, double expCC, double expError, bool displayProcess)
{
	// make sure the graph have more than three nodes
	if (expSize < 3)
		return LESS_THAN_THREE_NODE;

	// initialize it to complete graph state
	double actError		= 0;
	m_GraphSize			= expSize;
	m_EdgeNum			= (expSize-1)*expSize / 2;
	m_Triangles			= (expSize*expSize*expSize - 3*expSize*expSize + 2*expSize)/6; // fomular to calculate how many triangles in the complete graph
	m_Triangles			= m_Triangles*3; // The clustering coefficient is calculated by each nodes, so one triangle will be count as 3 times for 3 vertex.
	m_Triplets			= m_Triangles;
	Li_Node* tmpNode	= NULL;
	m_TotalCC			= 1.0;

	float tmp = m_Triangles;
	
	// for initialize the position as Circle by default,
	// didn't call the method because I merged the two loop together to save time complecity
	int num = expSize;
	// Circle layout
	int radius = num * 10;
	if (radius < 50)
		radius = 50;
	else if (radius > 200)
		radius = 200;

	int centerx = radius + 100;
	int centery = radius + 100;

	int numPerC = num;
	if (numPerC > NUMPERC)
		numPerC = NUMPERC;

	bool screw = false;
	if (num > numPerC)
		screw = true;

	// generate clustering coefficient backward
	// 1. create a complete graph
	
	// // 1-2. add the rest nodes and randomly link it to one of the exist node
	for (int i = 0; i < expSize; i++)
	{
		tmpNode = new Li_Node(i);			// Build an Node and input nod id
		m_Nodes.push_back(tmpNode);			// add the Node in to the Nodes vector
		tmpNode = NULL;						// free the pointer here, prevent protential crash later

		// connect to 
		// @TODO
	}
	// end for
	// after Step 1. the local clustering coefficient of each node in the graph should be all 0

	return 0;
}
*/

// Create certain size of Graph with expected Clustering Coefficient within expected Error, and expected Degree
double Li_Graph::fn_GenerateGraph(int expSize, double expCC, double expError, int expDegree, bool displayProcess)
{
	// make sure the graph have more than three nodes
	if (expSize < 3)
		return LESS_THAN_THREE_NODE;

	double actError		= 0;
	double absError		= 0;
	m_GraphSize			= 0;
	m_EdgeNum			= 0;
	m_Triplets			= 0;
	m_Triangles			= 0;
	Li_Node* tmpNode	= NULL;
	m_Transitivity		= 0.0;
	
	int totalEdges = expDegree * expSize / 2;

	// for initialize the position as Circle by default,
	// didn't call the method because I merged the two loop together to save time complecity
	int num = expSize;
	// Circle layout
	int radius = num * 10;
	if (radius < 50)
		radius = 50;
	else if (radius > 200)
		radius = 200;

	int centerx = radius + 100;
	int centery = radius + 100;

	int numPerC = num;
	if (numPerC > NUMPERC)
		numPerC = NUMPERC;

	bool screw = false;
	if (num > numPerC)
		screw = true;

	// Step 1. Generate a size N graph with size*average degree Edges
	/*
	// // step 1.1. generate an empty graph
	for (int i = 0; i < expSize; i++)
	{
		tmpNode = new Li_Node(i);			// Build an Node and input nod id
		m_Nodes.push_back(tmpNode);			// add the Node in to the Nodes vector
		tmpNode = NULL;						// free the pointer here, prevent protential crash later

		// increase the graph size
		m_GraphSize++;

		// initialize the position as Circle by default,
		// didn't call the method because I merged the two loop together to save time complecity
		m_Nodes[i]->m_PosX = centerx + radius *(float)cos(2 * PI * i / numPerC);
		m_Nodes[i]->m_PosY = centery + radius *(float)sin(2 * PI * i / numPerC);
		if (screw)
			radius++;
	}

	// // Step 1.2. add the edges
	// // // Step 1.2.1. connect the graph nodes first
	// this will make sure every node at least connected to another node;
	for (int i = 0; i < expSize; i++)
	{
		int tmpLink = 0;
		// the node to link must not be him self or already have the same link
		bool linkable = false;
		while (!linkable)
		{
			tmpLink = rand()%m_GraphSize;
			if (tmpLink != i)
				linkable = true;
			for (unsigned int j = 0; j < m_Nodes[i]->m_Conn.size(); j++)
			{
				if (m_Nodes[i]->m_Conn[j] == m_Nodes[tmpLink])
					linkable = false;
			}
		}
		m_Nodes[i]->m_Conn.push_back(m_Nodes[tmpLink]);
		m_Nodes[tmpLink]->m_Conn.push_back(m_Nodes[i]);
		// increase the local degree of the two node by one;
		m_Nodes[i]->m_LocDegree++;
		m_Nodes[tmpLink]->m_LocDegree++;

		// increase the edges
		m_EdgeNum++;
		// increase corresponding triplets
		m_Triplets += (m_Nodes[i]->m_Conn.size() - 1);
		m_Triplets += (m_Nodes[tmpLink]->m_Conn.size() - 1);
		// the number of shared neighbour of these two nodes times 3 is the number of the triangles
		int tmpTriangle = 0;
		for (unsigned int j = 0; j < m_Nodes[i]->m_Conn.size(); j++)
		{
			for (unsigned int k = 0; k < m_Nodes[tmpLink]->m_Conn.size(); k++)
			{
				if (m_Nodes[i]->m_Conn[j] == m_Nodes[tmpLink]->m_Conn[k])
				{
					tmpTriangle++;
					// // // // update the local clustering coefficient for the updated nodes. // // // //
					// update the number of the connections of the Node's connection cluster
					// realize the selected Node is not the only Node that influnced by the adding edge,
					// all the Nodes that connected to the two newly linked Nodes have to update their local clustering coefficient
					// so we have to go through all the nodes that may affected to find out which one needs to be update.
					// *1. add 1 to it cluster's connection, because thoes tow nodes are same, so edit 1 time
					m_Nodes[i]->m_Conn[j]->m_SubGraphConn++; // 
					// *2. compute the new local clustering coefficient
					int tmpDegree = m_Nodes[i]->m_Conn[j]->m_Conn.size();
					int tmpMaxConn = tmpDegree * (tmpDegree - 1) / 2;
					int tmpCurConn = m_Nodes[i]->m_Conn[j]->m_SubGraphConn;
					m_Nodes[i]->m_Conn[j]->m_LocCC =  tmpCurConn / tmpMaxConn;
				}
			}
			// end for k
		}
		// end for j

		// update the Triangles
		m_Triangles += tmpTriangle*3;
	}
	// after 1.2.1. the average degree should be 2
	*/

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

		// initialize the position as Circle by default,
		// didn't call the method because I merged the two loop together to save time complecity
		m_Nodes[i]->m_PosX = centerx + radius *(float)cos(2 * PI * i / numPerC);
		m_Nodes[i]->m_PosY = centery + radius *(float)sin(2 * PI * i / numPerC);
		if (screw)
			radius++;
	}
	// end for

	// now we have 3 nodes, with 2 links, linked as a line, which includes 1 triplets
	m_GraphSize	= 3;
	m_EdgeNum	= 2;
	m_Triplets	= 1;

	// // 1-2.1 add the rest nodes and randomly link it to one of the exist node
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
		m_EdgeNum++;
		// increase corresponding triplets
		m_Triplets += (m_Nodes[tmpLink]->m_Conn.size() - 1);

		// initialize the position as Circle by default,
		// didn't call the method because I merged the two loop together to save time complecity
		m_Nodes[i]->m_PosX = centerx + radius *(float)cos(2 * PI * i / numPerC);
		m_Nodes[i]->m_PosY = centery + radius *(float)sin(2 * PI * i / numPerC);
		if (screw)
			radius++;
	}
	// end for
	// after Step 1.2.1 the local clustering coefficient of each node in the graph should be all 0
	
	// // // Step 1.2.2. add the rest Edges
	for (int i = m_EdgeNum; i < totalEdges; i++)
	{
		// randomly take the first entrance
		int tmpE1 = rand()%m_GraphSize;

		int n1 = 0; // control the loop of tmpE1, to finish the program while every node is fully linked
		while (n1 < m_GraphSize - 1 && m_Nodes[tmpE1]->m_Conn.size() == (unsigned int)m_GraphSize - 1)
		{
			// if tmpE1 is full but there are still other nodes uncheck, move it to it's next node in the vecotr
			tmpE1++;
			tmpE1 = tmpE1 % m_GraphSize;
			n1++;
		}
		
		// after this, we have got an valid E1, and we know all of the n1 nodes before tmpE1 are full

		// if all the nodes are full, just break out
		if (n1 == m_GraphSize - 1)
			break;

		// tmp enterance 2, could get rid of the full links we marks.
		// tmpE2 take a random node between the graphsize - n1 nodes after tmpE1, because rand could be 0, so we use tmpE1+1 here
		int tmpE2 = (tmpE1 + 1 + rand()%(m_GraphSize - n1)) % m_GraphSize;
		int n2 = n1; // control the loop of tmpE2, to finish the program while every node is fully linked

		// make sure they are not already linked
		bool linkable = false;
		while ( (n2 < m_GraphSize - 1) && (!linkable) )
		{
			// the node is not full
			if (m_Nodes[tmpE2]->m_Conn.size() < (unsigned int)m_GraphSize - 1)
				linkable = true;

			// now check if it already linked, or it just himself
			for (unsigned int j = 0; j < m_Nodes[tmpE1]->m_Conn.size(); j++)
			{
				if (m_Nodes[tmpE1]->m_Conn[j] == m_Nodes[tmpE2] || m_Nodes[tmpE1] == m_Nodes[tmpE2])
					linkable = false;
			}
			// end for j

			// if tmpE1 is full but there are still other nodes uncheck, 
			// or tmpE1 and tmpE2 already linked, move it to it's next node in the vecotr
			if (!linkable)
			{
				tmpE2++;
				tmpE2 = tmpE2 % m_GraphSize;
				// if the tmpE2 going back to where E1 already searched, just jump out this area
				if (tmpE2 >= tmpE1 - (n1-1) && tmpE2 <= tmpE1)
					tmpE2 = tmpE1+1;
				n2++;
			}
		}
		// end while

		// after this, we have two valid nodes tmpE1 and tmpE2 are ready to connect
		m_Nodes[tmpE1]->m_Conn.push_back(m_Nodes[tmpE2]);
		m_Nodes[tmpE2]->m_Conn.push_back(m_Nodes[tmpE1]);
		// increase the edges
		m_EdgeNum++;
		// increase corresponding triplets
		m_Triplets += (m_Nodes[tmpE1]->m_Conn.size() - 1);
		m_Triplets += (m_Nodes[tmpE2]->m_Conn.size() - 1);
		// the number of shared neighbour of these two nodes times 3 is the number of the triangles
		int tmpTriangle = 0;
		for (unsigned int j = 0; j < m_Nodes[tmpE1]->m_Conn.size(); j++)
		{
			for (unsigned int k = 0; k < m_Nodes[tmpE2]->m_Conn.size(); k++)
			{
				if (m_Nodes[tmpE1]->m_Conn[j] == m_Nodes[tmpE2]->m_Conn[k])
				{
					tmpTriangle++;
					// // // // update the local clustering coefficient for the updated nodes. // // // //
					// update the number of the connections of the Node's connection cluster
					// realize the selected Node is not the only Node that influnced by the adding edge,
					// all the Nodes that connected to the two newly linked Nodes have to update their local clustering coefficient
					// so we have to go through all the nodes that may affected to find out which one needs to be update.
					// *1. add 1 to it cluster's connection, because thoes tow nodes are same, so edit 1 time
					m_Nodes[tmpE1]->m_Conn[j]->m_SubGraphConn++; // 
					// *2. compute the new local clustering coefficient
					int tmpDegree = m_Nodes[tmpE1]->m_Conn[j]->m_Conn.size();
					int tmpMaxConn = tmpDegree * (tmpDegree - 1) / 2;
					int tmpCurConn = m_Nodes[tmpE1]->m_Conn[j]->m_SubGraphConn;
					m_Nodes[tmpE1]->m_Conn[j]->m_LocCC =  tmpCurConn / tmpMaxConn;
				}
			}
			// end for k
		}
		// end for j

		// update the Triangles
		m_Triangles += tmpTriangle*3;
	}
	// end for i
	
	// calculate the Total Clustering coefficient
	// compute the total clustering coefficient and Error
	m_Transitivity	= double(m_Triangles) / double(m_Triplets);
	actError		= expCC - m_Transitivity;
	absError		= abs(actError);

	// setp 2. now looking for the clustering coefficient

	// a huriastic approach
	while (absError > expError)
	{
		// 1, remove an edge
		int pNode1 = 0, pNode2 = 0, pConn1 = 0, pConn2 = 0; // conn is used to erase the link later
		// the potential attratributes
		int pTriangles = m_Triangles;
		int pTriplets = m_Triplets;
		double pTotalCC = 0, pError = 0;
		// the minimum attratributes
		int mTriangles = 0, mTriplets = 0;
		double mTotalCC = 0, mError = 0;

		bool isthefirst = true;

		for (unsigned int i = 0; i < m_Nodes.size(); i++)
		{
			for (unsigned int j = 0; j < m_Nodes[i]->m_Conn.size(); j++)
			{
				// test if this is the only way between two point
				vector<Li_Node*> tmpNodeVec;

				for (unsigned int k = 0; k < m_Nodes.size(); k++)
					m_Nodes[k]->m_isVisited = false;

				// 设置i已经访问过，这样可以避免重新读取这条应该被删除的连接
				m_Nodes[i]->m_isVisited = true;
				// 将i点除了j点以外的其它连接都加到栈里
				for (unsigned int k = 0; k < m_Nodes[i]->m_Conn.size(); k++)
				{
					if (k != j)
					{
						tmpNodeVec.push_back(m_Nodes[i]->m_Conn[k]);
						m_Nodes[i]->m_Conn[k]->m_isVisited = true;
					}
				}
				
				int numofpath = 0;
				while (tmpNodeVec.size() != 0)
				{
					Li_Node* curNode;
					curNode = tmpNodeVec.back();

					tmpNodeVec.pop_back();
					if (curNode == m_Nodes[i]->m_Conn[j])
					{
						numofpath++;
						// found it
						break;
					}

					for(unsigned int k = 0; k < curNode->m_Conn.size(); k++)
					{
						if (!curNode->m_Conn[k]->m_isVisited)
						{
							tmpNodeVec.push_back(curNode->m_Conn[k]);
							curNode->m_Conn[k]->m_isVisited = true;
						}
					}
					// end for
				}
				// end while

				if (numofpath == 0) // no other path
				{
					// this can not be deleted
				}
				else
				{
					// first, get the first potentialError, for further compairation
					if (isthefirst)
					{
						// decrease corresponding triplets
						pTriplets = m_Triplets - (m_Nodes[i]->m_Conn.size() - 1);
						pTriplets -= (m_Nodes[i]->m_Conn[j]->m_Conn.size() - 1);
						// the number of shared neighbour of these two nodes times 3 is the number of the triangles
						int tmpTriangle = 0;
						for (unsigned int m = 0; m < m_Nodes[i]->m_Conn.size(); m++)
						{
							for (unsigned int n = 0; n < m_Nodes[i]->m_Conn[j]->m_Conn.size(); n++)
							{
								if (m_Nodes[i]->m_Conn[m] == m_Nodes[i]->m_Conn[j]->m_Conn[n])
								{
									tmpTriangle++;
								}
							}
							// end for k
						}
						// end for j

						// update the Triangles
						pTriangles = m_Triangles - tmpTriangle*3;

						pTotalCC = double(pTriangles) / double(pTriplets);
						pError = abs(expCC - pTotalCC);

						pNode1 = i;
						pConn1 = j;
						pNode2 = m_Nodes[i]->m_Conn[j]->m_ID;
						for (unsigned int k = 0; k < m_Nodes[pNode2]->m_Conn.size(); k++)
						{
							if (m_Nodes[pNode2]->m_Conn[k] == m_Nodes[i])
							{
								pConn2 = k;
								break;
							}
						}
						mTriplets = pTriplets;
						mTriangles = pTriangles;
						mTotalCC = pTotalCC;
						mError = pError;

						isthefirst = false;
					}
					else
					{
						// if not the first one, compare the error
						// realize because the loop is form 0 to size,
						// we do not need to test a node who connect to a smaller ID, because this link is already tested before
						if (m_Nodes[i]->m_ID < m_Nodes[i]->m_Conn[j]->m_ID)
						{
							// decrease corresponding triplets
							pTriplets = m_Triplets - (m_Nodes[i]->m_Conn.size() - 1);
							pTriplets -= (m_Nodes[i]->m_Conn[j]->m_Conn.size() - 1);
							// the number of shared neighbour of these two nodes times 3 is the number of the triangles
							int tmpTriangle = 0;
							for (unsigned int m = 0; m < m_Nodes[i]->m_Conn.size(); m++)
							{
								for (unsigned int n = 0; n < m_Nodes[i]->m_Conn[j]->m_Conn.size(); n++)
								{
									if (m_Nodes[i]->m_Conn[m] == m_Nodes[i]->m_Conn[j]->m_Conn[n])
									{
										tmpTriangle++;
									}
								}
								// end for n
							}
							// end for m

							// update the Triangles
							pTriangles = m_Triangles - tmpTriangle*3;

							pTotalCC = double(pTriangles) / double(pTriplets);
							pError = abs(expCC - pTotalCC);

							// swap if it is better
							if (pError < mError)
							{
								pNode1 = i;
								pConn1 = j;
								pNode2 = m_Nodes[i]->m_Conn[j]->m_ID;
								for (unsigned int k = 0; k < m_Nodes[pNode2]->m_Conn.size(); k++)
								{
									if (m_Nodes[pNode2]->m_Conn[k] == m_Nodes[i])
									{
										pConn2 = k;
										break;
									}
								}
								mTriplets = pTriplets;
								mTriangles = pTriangles;
								mTotalCC = pTotalCC;
								mError = pError;
							}
							// end if
						}
						// end if
					}
					// end if / else (the first)
				}
				// end if(numofpath == 0)
			}
			// end for j
		}
		// end for i

		// after the loop, we got the most close change
		// then do it

		// remove the link
		m_Nodes[pNode1]->m_Conn.erase(m_Nodes[pNode1]->m_Conn.begin()+pConn1);
		m_Nodes[pNode2]->m_Conn.erase(m_Nodes[pNode2]->m_Conn.begin()+pConn2);

		// then copy the corresponding attributes
		// I do the recalculate because I want to also calculate the local attributes during the progress of calculation
		// so I just comment this copy approach.
		/*
		m_Triplets = mTriplets;
		m_Triangles = mTriangles;
		m_TotalCC = mTotalCC;
		actError = expCC - m_TotalCC;
		absError = abs(actError);
		*/

		// decrease corresponding triplets
		m_Triplets -= m_Nodes[pNode1]->m_Conn.size();
		m_Triplets -= m_Nodes[pNode2]->m_Conn.size();
		// the number of shared neighbour of these two nodes times 3 is the number of the triangles
		int tmpTriangle = 0;
		for (unsigned int j = 0; j < m_Nodes[pNode1]->m_Conn.size(); j++)
		{
			for (unsigned int k = 0; k < m_Nodes[pNode2]->m_Conn.size(); k++)
			{
				if (m_Nodes[pNode1]->m_Conn[j] == m_Nodes[pNode2]->m_Conn[k])
				{
					tmpTriangle++;
					// // // // update the local clustering coefficient for the updated nodes. // // // //
					// update the number of the connections of the Node's connection cluster
					// realize the selected Node is not the only Node that influnced by the adding edge,
					// all the Nodes that connected to the two newly linked Nodes have to update their local clustering coefficient
					// so we have to go through all the nodes that may affected to find out which one needs to be update.
					// *1. add 1 to it cluster's connection, because thoes tow nodes are same, so edit 1 time
					m_Nodes[pNode1]->m_Conn[j]->m_SubGraphConn--; // 
					// *2. compute the new local clustering coefficient
					int tmpDegree = m_Nodes[pNode1]->m_Conn[j]->m_Conn.size();
					int tmpMaxConn = tmpDegree * (tmpDegree - 1) / 2;
					int tmpCurConn = m_Nodes[pNode1]->m_Conn[j]->m_SubGraphConn;
					m_Nodes[pNode1]->m_Conn[j]->m_LocCC =  tmpCurConn / tmpMaxConn;
				}
			}
			// end for k
		}
		// end for j

		// update the Triangles
		m_Triangles -= tmpTriangle*3;
		
		// calculate the Total Clustering coefficient
		// compute the total clustering coefficient and Error
		m_Transitivity	= double(m_Triangles) / double(m_Triplets);
		actError		= expCC - m_Transitivity;
		absError		= abs(actError);

		// 2. add a new edge
		// reset the temp attributes
		pNode1 = 0;
		pNode2 = 0;
		// do not need conn here
		//pConn1 = 0;
		//pConn2 = 0; // conn is used to erase the link later
		// the potential attratributes
		pTriangles = m_Triangles;
		pTriplets = m_Triplets;
		pTotalCC = 0;
		pError = 0;
		// the minimum attratributes
		mTriangles = 0;
		mTriplets = 0;
		mTotalCC = 0;
		mError = 0;

		// find the first unconnected pair
		isthefirst = true;

		// go through each pair
		for (unsigned int i = 0; i < m_Nodes.size()-1; i++)
		{
			for (unsigned int j = i+1; j < m_Nodes.size(); j++)
			{
				// make sure they haven't connected yet
				bool isconnected = false;
				for (unsigned int k = 0; k < m_Nodes[i]->m_Conn.size(); k++)
				{
					if (m_Nodes[i]->m_Conn[k] == m_Nodes[j])
					{
						isconnected = true;
						break;
					}
					// end if
				}
				// end for k
				
				// only try those who did not connected yet
				if (!isconnected)
				{
					// assign the minimum error to the first pair
					if (isthefirst)
					{
						// increase corresponding triplets
						pTriplets = m_Triplets + m_Nodes[i]->m_Conn.size();
						pTriplets += m_Nodes[j]->m_Conn.size();

						// the number of shared neighbour of these two nodes times 3 is the number of the triangles
						int tmpTriangle = 0;
						for (unsigned int m = 0; m < m_Nodes[i]->m_Conn.size(); m++)
						{
							for (unsigned int n = 0; n < m_Nodes[j]->m_Conn.size(); n++)
							{
								if (m_Nodes[i]->m_Conn[m] == m_Nodes[j]->m_Conn[n])
								{
									tmpTriangle++;
								}
							}
							// end for n
						}
						// end for m

						// update the Triangles
						pTriangles = m_Triangles + tmpTriangle*3;pTotalCC = double(pTriangles) / double(pTriplets);
						pError = abs(expCC - pTotalCC);

						pNode1 = i;
						pNode2 = j;
	
						mTriplets = pTriplets;
						mTriangles = pTriangles;
						mTotalCC = pTotalCC;
						mError = pError;

						isthefirst = false;
					}
					else
					{
						// increase corresponding triplets
						pTriplets = m_Triplets + m_Nodes[i]->m_Conn.size();
						pTriplets += m_Nodes[j]->m_Conn.size();

						// the number of shared neighbour of these two nodes times 3 is the number of the triangles
						int tmpTriangle = 0;
						for (unsigned int m = 0; m < m_Nodes[i]->m_Conn.size(); m++)
						{
							for (unsigned int n = 0; n < m_Nodes[j]->m_Conn.size(); n++)
							{
								if (m_Nodes[i]->m_Conn[m] == m_Nodes[j]->m_Conn[n])
								{
									tmpTriangle++;
								}
							}
							// end for n
						}
						// end for m

						// update the Triangles
						pTriangles = m_Triangles + tmpTriangle*3;pTotalCC = double(pTriangles) / double(pTriplets);
						pError = abs(expCC - pTotalCC);

						// swap if it is better
						if (pError < mError)
						{
							pNode1 = i;
							pNode2 = j;
	
							mTriplets = pTriplets;
							mTriangles = pTriangles;
							mTotalCC = pTotalCC;
							mError = pError;
						}
						// end if
					}
					// end if isthefirst / else
				}
				// end if !isconnected
			}
			// end for j
		}
		// end for i

		// after the loop, we got the most close change
		// then do it
		// add the link
		m_Nodes[pNode1]->m_Conn.push_back(m_Nodes[pNode2]);
		m_Nodes[pNode2]->m_Conn.push_back(m_Nodes[pNode1]);

		// now modify the actual attribute
		// first the triplets
		m_Triplets += (m_Nodes[pNode1]->m_Conn.size() - 1);
		m_Triplets += (m_Nodes[pNode2]->m_Conn.size() - 1);
		// the triangles
		// the number of shared neighbour of these two nodes times 3 is the number of the triangles
		tmpTriangle = 0;
		for (unsigned int j = 0; j < m_Nodes[pNode1]->m_Conn.size(); j++)
		{
			for (unsigned int k = 0; k < m_Nodes[pNode2]->m_Conn.size(); k++)
			{
				if (m_Nodes[pNode1]->m_Conn[j] == m_Nodes[pNode2]->m_Conn[k])
				{
					tmpTriangle++;
					// // // // update the local clustering coefficient for the updated nodes. // // // //
					// update the number of the connections of the Node's connection cluster
					// realize the selected Node is not the only Node that influnced by the adding edge,
					// all the Nodes that connected to the two newly linked Nodes have to update their local clustering coefficient
					// so we have to go through all the nodes that may affected to find out which one needs to be update.
					// *1. add 1 to it cluster's connection, because thoes tow nodes are same, so edit 1 time
					m_Nodes[pNode1]->m_Conn[j]->m_SubGraphConn++; // 
					// *2. compute the new local clustering coefficient
					int tmpDegree = m_Nodes[pNode1]->m_Conn[j]->m_Conn.size();
					int tmpMaxConn = tmpDegree * (tmpDegree - 1) / 2;
					int tmpCurConn = m_Nodes[pNode1]->m_Conn[j]->m_SubGraphConn;
					m_Nodes[pNode1]->m_Conn[j]->m_LocCC =  tmpCurConn / tmpMaxConn;
				}
			}
			// end for k
		}
		// end for j

		// update the Triangles
		m_Triangles += tmpTriangle*3;

		// calculate the Total Clustering coefficient
		// compute the total clustering coefficient and Error
		m_Transitivity	= double(m_Triangles) / double(m_Triplets);
		actError		= expCC - m_Transitivity;
		absError		= abs(actError);
		
		if (displayProcess)
			std::cout<<expSize<<", "<<m_EdgeNum<<", "<<m_Triangles<<" / "<<m_Triplets<<" : "<<m_Transitivity<<endl;
	}
	// end while (absError > expError)

	// This approach is too slow for generate large graph, use the one above
	/*
	while (absError > expError)
	{
		// // step 2.1. first, remove an edge;
		//if (actError >= 0)
			// the actual CC is smaller than expected
		{
			// we want to decrease as less triangles as possible
			// remove a link from the lower loc CC Node, because its neighbour is unlikely to connected to eachother
			// so pick a node with Loc CC lower than the total CC
			// pick a entrence
			int tmpEnt = rand()%m_GraphSize;
			// if it only have one link or its Loc CC is heigher than total CC, move to next
			while (m_Nodes[tmpEnt]->m_Conn.size() == 1)// || m_Nodes[tmpEnt]->m_LocCC >= m_TotalCC)
			{
				tmpEnt++;
				tmpEnt = tmpEnt%m_GraphSize;
			}
			// now remove a link from tmeEnt
			// pop will keep the ealer added link untouched, so we need to use another rand number to delete form the vector
			//m_Nodes[tmpEnt]->m_Conn.pop_back();
			int tmpEnt2 = rand()%m_Nodes[tmpEnt]->m_Conn.size();
			
			// decrease corresponding triplets
			m_Triplets -= (m_Nodes[tmpEnt]->m_Conn.size() - 1);
			m_Triplets -= (m_Nodes[tmpEnt]->m_Conn[tmpEnt2]->m_Conn.size() - 1);

			// the number of shared neighbour of these two nodes times 3 is the number of the triangles
			int tmpTriangle = 0;
			for (unsigned int j = 0; j < m_Nodes[tmpEnt]->m_Conn.size(); j++)
			{
				for (unsigned int k = 0; k < m_Nodes[tmpEnt]->m_Conn[tmpEnt2]->m_Conn.size(); k++)
				{
					if (m_Nodes[tmpEnt]->m_Conn[j] == m_Nodes[tmpEnt]->m_Conn[tmpEnt2]->m_Conn[k])
					{
						tmpTriangle++;
						// // // // update the local clustering coefficient for the updated nodes. // // // //
						// update the number of the connections of the Node's connection cluster
						// realize the selected Node is not the only Node that influnced by the adding edge,
						// all the Nodes that connected to the two newly linked Nodes have to update their local clustering coefficient
						// so we have to go through all the nodes that may affected to find out which one needs to be update.
						// *1. add 1 to it cluster's connection, because thoes tow nodes are same, so edit 1 time
						m_Nodes[tmpEnt]->m_Conn[j]->m_SubGraphConn++; // 
						// *2. compute the new local clustering coefficient
						int tmpDegree = m_Nodes[tmpEnt]->m_Conn[j]->m_Conn.size();
						int tmpMaxConn = tmpDegree * (tmpDegree - 1) / 2;
						int tmpCurConn = m_Nodes[tmpEnt]->m_Conn[j]->m_SubGraphConn;
						m_Nodes[tmpEnt]->m_Conn[j]->m_LocCC =  tmpCurConn / tmpMaxConn;
					}
				}
				// end for k
			}
			// end for j

			// update the Triangles
			m_Triangles -= tmpTriangle*3;

			Li_Node *tmpNode = m_Nodes[tmpEnt]->m_Conn[tmpEnt2];
			
			// remove the tmp node 2 from the connection vector first
			m_Nodes[tmpEnt]->m_Conn.erase(m_Nodes[tmpEnt]->m_Conn.begin()+tmpEnt2);

			int tmpIndex = 0;
			// find the index of current node in the disconnected node
			for (unsigned int i = 0; i < tmpNode->m_Conn.size(); i++)
			{
				if (tmpNode->m_Conn[i] == m_Nodes[tmpEnt])
					tmpIndex = i;
			}

			// we dont remove it because to keep the map connected, we need to get it another link;

			// if the original node have another neighbour connected to it already, the graph is already connected,
			// so we connect it to another random node
			// else we connect it to any neighbour of the original node;
			// so, instead we remove it and push a new one, we could just usethe original one, so we can save a shuffle
			// due to the vector is an array implementation

			bool isconnected = false;
			for (unsigned int i = 0; i < m_Nodes[tmpEnt]->m_Conn.size(); i++)
			{
				for (unsigned int j=0; j < m_Nodes[tmpEnt]->m_Conn[i]->m_Conn.size(); j++)
				{
					if (m_Nodes[tmpEnt]->m_Conn[i]->m_Conn[j] == tmpNode)
					{
						isconnected = true;
						break;
					}
				}

				if (isconnected)
					break;
			}
			if (!isconnected)
			{
				// reuse tmpEnt2 since it is nolonger effective
				tmpEnt2 = rand()%m_Nodes[tmpEnt]->m_Conn.size();
				tmpNode->m_Conn[tmpIndex] = m_Nodes[tmpEnt]->m_Conn[tmpEnt2];
				m_Nodes[tmpEnt]->m_Conn[tmpEnt2]->m_Conn.push_back(tmpNode);

				// increase corresponding triplets
				m_Triplets += (tmpNode->m_Conn.size() - 1);
				m_Triplets += (m_Nodes[tmpEnt]->m_Conn[tmpEnt2]->m_Conn.size() - 1);
				// the number of shared neighbour of these two nodes times 3 is the number of the triangles
				int tmpTriangle = 0;
				for (unsigned int j = 0; j < tmpNode->m_Conn.size(); j++)
				{
					for (unsigned int k = 0; k < m_Nodes[tmpEnt]->m_Conn[tmpEnt2]->m_Conn.size(); k++)
					{
						if (tmpNode->m_Conn[j] == m_Nodes[tmpEnt]->m_Conn[tmpEnt2]->m_Conn[k])
						{
							tmpTriangle++;
							// // // // update the local clustering coefficient for the updated nodes. // // // //
							// update the number of the connections of the Node's connection cluster
							// realize the selected Node is not the only Node that influnced by the adding edge,
							// all the Nodes that connected to the two newly linked Nodes have to update their local clustering coefficient
							// so we have to go through all the nodes that may affected to find out which one needs to be update.
							// *1. add 1 to it cluster's connection, because thoes tow nodes are same, so edit 1 time
							tmpNode->m_Conn[j]->m_SubGraphConn++; // 
							// *2. compute the new local clustering coefficient
							int tmpDegree = tmpNode->m_Conn[j]->m_Conn.size();
							int tmpMaxConn = tmpDegree * (tmpDegree - 1) / 2;
							int tmpCurConn = tmpNode->m_Conn[j]->m_SubGraphConn;
							tmpNode->m_Conn[j]->m_LocCC =  tmpCurConn / tmpMaxConn;
						}
					}
					// end for k
				}
				// end for j

				// update the Triangles
				m_Triangles += tmpTriangle*3;
			}
			else
			{
				// remove the node from the connection vector
				tmpNode->m_Conn.erase(tmpNode->m_Conn.begin()+tmpIndex);

				// then random add a link

				// reuse tmpEnt since it is nolonger effective
				tmpEnt = rand()%m_Nodes.size();
				int n1 = 0; // control the loop of tmpE1, to finish the program while every node is fully linked
				while (n1 < m_GraphSize - 1 && m_Nodes[tmpEnt]->m_Conn.size() == (unsigned int)m_GraphSize - 1)
				{
					// if tmpE1 is full but there are still other nodes uncheck, move it to it's next node in the vecotr
					tmpEnt++;
					tmpEnt = tmpEnt % m_GraphSize;
					n1++;
				}
		
				// after this, we have got an valid E1, and we know all of the n1 nodes before tmpE1 are full

				// reuse tmpEnt2 since it is nolonger effective
				tmpEnt2 = rand()%m_Nodes.size();

				// make sure they are not already linked
				int n2 = 0;
				bool linkable = false;
				while ( (n2 < m_GraphSize - 1) && (!linkable) )
				{
					// the node is not full
					if (m_Nodes[tmpEnt2]->m_Conn.size() < (unsigned int)m_GraphSize - 1)
						linkable = true;

					// now check if it already linked, or it just himself
					for (unsigned int j = 0; j < m_Nodes[tmpEnt]->m_Conn.size(); j++)
					{
						if (m_Nodes[tmpEnt]->m_Conn[j] == m_Nodes[tmpEnt2] || m_Nodes[tmpEnt] == m_Nodes[tmpEnt2])
							linkable = false;
					}
					// end for j

					// if tmpE1 is full but there are still other nodes uncheck, 
					// or tmpE1 and tmpE2 already linked, move it to it's next node in the vecotr
					if (!linkable)
					{
						tmpEnt2++;
						tmpEnt2 = tmpEnt2 % m_GraphSize;
						// if the tmpE2 going back to where E1 already searched, just jump out this area
						if (tmpEnt2 >= tmpEnt - (n1-1) && tmpEnt2 <= tmpEnt)
							tmpEnt2 = tmpEnt+1;
						n2++;
					}
				}
				// end while
				
				// after this, we have two valid nodes tmpE1 and tmpE2 are ready to connect
				m_Nodes[tmpEnt]->m_Conn.push_back(m_Nodes[tmpEnt2]);
				m_Nodes[tmpEnt2]->m_Conn.push_back(m_Nodes[tmpEnt]);

				// increase corresponding triplets
				m_Triplets += (m_Nodes[tmpEnt]->m_Conn.size() - 1);
				m_Triplets += (m_Nodes[tmpEnt2]->m_Conn.size() - 1);
				// the number of shared neighbour of these two nodes times 3 is the number of the triangles
				int tmpTriangle = 0;
				for (unsigned int j = 0; j < m_Nodes[tmpEnt]->m_Conn.size(); j++)
				{
					for (unsigned int k = 0; k < m_Nodes[tmpEnt2]->m_Conn.size(); k++)
					{
						if (m_Nodes[tmpEnt]->m_Conn[j] == m_Nodes[tmpEnt2]->m_Conn[k])
						{
							tmpTriangle++;
							// // // // update the local clustering coefficient for the updated nodes. // // // //
							// update the number of the connections of the Node's connection cluster
							// realize the selected Node is not the only Node that influnced by the adding edge,
							// all the Nodes that connected to the two newly linked Nodes have to update their local clustering coefficient
							// so we have to go through all the nodes that may affected to find out which one needs to be update.
							// *1. add 1 to it cluster's connection, because thoes tow nodes are same, so edit 1 time
							m_Nodes[tmpEnt]->m_Conn[j]->m_SubGraphConn++; // 
							// *2. compute the new local clustering coefficient
							int tmpDegree = m_Nodes[tmpEnt]->m_Conn[j]->m_Conn.size();
							int tmpMaxConn = tmpDegree * (tmpDegree - 1) / 2;
							int tmpCurConn = m_Nodes[tmpEnt]->m_Conn[j]->m_SubGraphConn;
							m_Nodes[tmpEnt]->m_Conn[j]->m_LocCC =  tmpCurConn / tmpMaxConn;
						}
					}
					// end for k
				}
				// end for j

				// update the Triangles
				m_Triangles += tmpTriangle*3;
			}
			// end if/else
		}
		// end if (actError >= 0)

		// compute the total clustering coefficient and Error again for add edge
		m_TotalCC = double(m_Triangles) / double(m_Triplets);
		actError = expCC - m_TotalCC;
		absError = abs(actError);
		
		if (displayProcess)
			std::cout<<expSize<<", "<<m_EdgeNum<<", "<<m_Triangles<<" / "<<m_Triplets<<" : "<<m_TotalCC<<endl;
	}
	// end while
	*/

	return false;
}

// generate graph genetic
/*
void fn_GenerateGraphGA(int expSize)
{
	// Genetic parameters
	int population = 100;
	int maxgen = 500;

	int maxEdges = expSize*(expSize - 1) / 2;

	int *adjMatrix = new int[maxEdges];


	delete adjMatrix;
}
*/

// calculate graph attributes
void Li_Graph::fn_calGraphAttributes()
{
	int		sumDegree	= 0;	// used to calculate the average graph degree
	double	sumLocCC	= 0;	// used to calculate the average graph Clustering coefficient
	// initialize the graph attribute calculate sensitive data for the entire graph
	m_GraphSize	= (int)m_Nodes.size();
	m_EdgeNum		= 0;
	m_AvgDegree		= 0;
	m_AvgCC			= 0.0;
	m_Transitivity	= 0.0;

	m_Triplets		= 0;
	m_Triangles		= 0;

	if (!m_Nodes.empty())
	{
		// 1. go through every node
		for (int i = 0; i < (int)m_Nodes.size(); i++)
		{
			// initialize the graph attribute calculate sensitive data for the node
			m_Nodes[i]->m_LocDegree		= m_Nodes[i]->m_Conn.size();
			m_Nodes[i]->m_LocCC			= 0.0;
			m_Nodes[i]->m_SubGraphConn	= 0;
			m_Nodes[i]->m_ID			= i;

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
		m_EdgeNum	= sumDegree / 2;
		// 3.2. get the average degree
		m_AvgDegree	= (double)sumDegree / m_GraphSize;
		// 3.3. get the average CC
		m_AvgCC		= sumLocCC / m_GraphSize;
		// 3.4. get the total CC
		if (m_Triplets != 0)
			m_Transitivity = (double)m_Triangles / m_Triplets;
	}
}

// save the Graph to a file
bool Li_Graph::fn_SaveGraph(char* filename)
{
	if (m_Nodes.empty())
		return false;

	if (strcmp(filename, "") == 0)
		filename = m_gpName;
	ofstream fout;
	fout.open(filename, ios::out);

	fout<<"Li_Generate_Graph:\n";
	fout<<m_Nodes.size()<<"\n";
	for (unsigned int i = 0; i < m_Nodes.size(); i++)
	{
		fout<<m_Nodes[i]->m_ID<<"\t"<<m_Nodes[i]->m_PosX<<"\t"<<m_Nodes[i]->m_PosY<<"\t"<<m_Nodes[i]->m_Conn.size()<<"\t{\t";
		for (unsigned int j = 0; j < m_Nodes[i]->m_Conn.size(); j++)
		{
			fout<<m_Nodes[i]->m_Conn[j]->m_ID<<"\t";
		}
		fout<<"};\n";
	}

	fout.close();
	return true;
}

// save the adjcency matrix to a file
bool Li_Graph::fn_SaveAdjMatrix(char* filename)
{
	if (m_Nodes.empty())
		return false;

	if (strcmp(filename, "") == 0)
		filename = m_gpName;
	ofstream fout;
	fout.open(filename, ios::out);

	fout<<"Li_Generate_Graph:\n";
	fout<<m_Nodes.size()<<"\n";
	for (unsigned int i = 0; i < m_Nodes.size(); i++)
	{
		for (unsigned int j = 0; j < m_Nodes.size(); j++)
		{
			if (fn_Islinked(m_Nodes[i], m_Nodes[j]))
			{
				fout<<1;
			}
			else
			{
				fout <<0;
			}
			fout<<" ";
		}
		fout<<"\n";
	}

	fout.close();
	return true;
}

// load the Graph from a file
bool Li_Graph::fn_LoadGraph(char* filename)
{
	if (strcmp(filename, "") == 0)
		filename = m_gpName;
	
	// still nothing, return false
	if (strcmp(filename, "") == 0)
		return false;

	ifstream fin;
	fin.open(filename, ios::in);

	if (!fin.is_open())
		return false;

	int numNodes, curNode, numConn, connNode;
	float xpos, ypos;
	char c; // temp save the characters
	//int pointerpos; // for test

	// first, check if it is the valid file
	string validationStr;
	//fin>>validationStr;
	getline(fin, validationStr,'\n');

	if (validationStr != "Li_Generate_Graph:")
		return false;

	// get the total number of nodes of the graph first
	fin>>numNodes;
	// create only nodes for the graph
	fn_CreateNodes(numNodes);

	// get the first current node,
	fin>>curNode;
	// get the node connection
	while(fin.good())
	{
		// get the position
		fin>>xpos>>ypos;
		// get the number of its connection, and a '{' character
		fin>>numConn>>c;
		// set position
		m_Nodes[curNode]->m_PosX = xpos;
		m_Nodes[curNode]->m_PosY = ypos;
		// create the link
		for (int i = 0; i < numConn; i++)
		{
			// get the connection destination
			fin >>connNode;
			m_Nodes[curNode]->m_Conn.push_back(m_Nodes[connNode]);
			//Li_Edge *tmpEdge = new Li_Edge(curNode, connNode);
			//m_Edges.push_back(tmpEdge);
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

void Li_Graph::fn_InitNodePos(GraphLayout layout)
{
	if (!m_Nodes.empty())
	{
		int num = m_Nodes.size();

		// Circle layout
		if (layout == L_CIRCLE)
		{
			int radius = num * 10;
			if (radius < 50)
				radius = 50;
			else if (radius > 200)
				radius = 200;

			int centerx = radius + 100;
			int centery = radius + 100;

			int numPerC = num;
			if (numPerC > NUMPERC)
				numPerC = NUMPERC;

			bool screw = false;
			if (num > numPerC)
				screw = true;

			for (int i = 0; i < num; i++)
			{
				//m_Nodes[i]->m_PosX = centerx + radius *(float)cos(2 * PI * i / numPerC);
				//m_Nodes[i]->m_PosY = centery + radius *(float)sin(2 * PI * i / numPerC);
				m_Nodes[i]->m_DestX = centerx + radius *(float)cos(2 * PI * i / numPerC);
				m_Nodes[i]->m_DestY = centery + radius *(float)sin(2 * PI * i / numPerC);
				if (screw)
					radius++;
				// end if
			}
			// end for
		}
		// end Circle layout

		// Square layout
		else if (layout == L_SQUARE)
		{
			int dim = (int)sqrt((double)num);
			for (int i = 0; i < num; i++)
			{
				//m_Nodes[i]->m_PosX = 100.0f + i % dim * 50;
				//m_Nodes[i]->m_PosY = 100.0f + i / dim * 50;
				m_Nodes[i]->m_DestX = 100.0f + i % dim * 50;
				m_Nodes[i]->m_DestY = 100.0f + i / dim * 50;
			}
		}
		// end Square layout
	}
}