#ifndef LI_NODE_H
#define LI_NODE_H

#include <vector>

using namespace std;

enum MOVE
{
	C = 0,
	D = 1
};

enum STRATEGY
{
	PUREC	= 0,
	PURED	= 1,
	TFT		= 2
};

class Li_Agent
{
public:
	Li_Agent(STRATEGY is = PUREC)
	{
		// Initialize the agent's strategy
		m_Strategy = is;
		m_UpdateStrategy = m_Strategy;
		switch (m_Strategy)
		{
		case PUREC:
			{
				m_Move = C;	// PUREC always Cooperate
			} break;
		case PURED:
			{
				m_Move = D;	// PURED always Defacte
			} break;
		case TFT:
			{
				m_Move = C;	// TFT strated at Cooperate
			} break;
		default:
			{
				m_Move = C;
			} break;
		}

		m_UpdateMove = m_Move;

		// initialize the total payoff
		m_TotalPayoff = 0.0;
	}

	// member variables ////
	MOVE		m_Move;				// move in one iteration for a strategy
	MOVE		m_UpdateMove;
	STRATEGY	m_Strategy;			// the strategy this Player plays
	STRATEGY	m_UpdateStrategy;	// a intermedia strategy for the calculating (make sure that all Players can update at the same time in evolution)
	double		m_TotalPayoff;		// total payoff of this player
	double		m_AveragePayoff;	// average payoff of this player
};

class Li_Node
{
public:
	// member functions ////
	Li_Node(int id)
	{
		// initialize as a agent
		m_Agent = new Li_Agent();

		// Initialize as a node
		m_ID			= id;
		m_LocDegree		= 0;
		m_LocCC			= 0.0;
		m_SubGraphConn	= 0;

		m_PosX			= 0.0;
		m_PosY			= 0.0;
		m_isSelected	= false;
	}

	~Li_Node()
	{
		m_Conn.clear();
		
		delete(m_Agent);
		m_Agent = NULL;

		// Initialize as a node
		m_ID			= 0;
		m_LocDegree		= 0;
		m_LocCC			= 0.0;
		m_SubGraphConn	= 0;
	}

	bool isconnectedto(Li_Node* node)
	{
		bool isconn = false;
		int tsize = m_Conn.size();
		for (int i = 0; i < tsize; i++)
		{
			if (m_Conn[i] == node)
			{
				isconn = true;
				break;
			}
		}

		return isconn;
	}

	// member variables ////
	Li_Agent* m_Agent;

	// Node attribute
	int		m_ID;				// The Node's ID, used to locate the agent in the vector
	vector<Li_Node*> m_Conn;	// graph connection

	int		m_LocDegree;		// local degree for one agent
	double	m_LocCC;			// local clustering coefficient for one agent
	int		m_SubGraphConn;		// the number of the connections of this agent's connection subGraph, used to compute the local clustering coefficient

	float	m_PosX;
	float	m_PosY;
	bool	m_isSelected;

	bool	m_isVisited;		// used to do the travel
};

#endif