#ifndef GAME_H
#define GAME_H

#include <fstream>
#include "../Li_Graph/Li_Node.h"

  /////////////////
 // Common Game //
/////////////////
class Li_Game
{
public:
	virtual void fn_Play(Li_Node &a, Li_Node &b){a; b;}
	virtual void fn_Evolve(Li_Node &a){}
};

  ////////////////////////
 // prisoner's Dilemma //
////////////////////////
class Li_PDGame : Li_Game
{
public:
	double R;
	double S;
	double T;
	double P;

	bool isSelfInteract;

	// for iterated strategy
	//bool ITERATE;
	//int iterateGen; // iterated generations, 0 no iterate, -1 infinite liternate

	//int TFTTRACE;

	Li_PDGame()
	{
		fn_LoadParams("GameSetting.ini");

		isSelfInteract = false;
	}

	bool fn_LoadParams(char* szFileName)
	{
		ifstream param(szFileName);
		//check file exists
		if (!param)
		{
			return false;
		}

		//load in from the file
		char ParamDescription[32];

		param >> ParamDescription;
		param >> R;	// Max Generations
		param >> ParamDescription;
		param >> S;	// Population Size
		param >> ParamDescription;
		param >> T;	// Max Generations
		param >> ParamDescription;
		param >> P;	// Population Size
		//param >> ParamDescription;
		//param >> ITERATE;
		//param >> ParamDescription;
		//param >> iterateGen;
		//param >> ParamDescription;
		//param >> TFTTRACE;

		param.close();

		return true;
	}

	void fn_Play(Li_Node *a, Li_Node *b, bool isDoubleDir)
	{
		// 0: Cooperate;
		// 1: Defecte

		if (a->m_Agent->m_Strategy == 0 && b->m_Agent->m_Strategy == 0)
		{
			a->m_Agent->m_TotalPayoff += R;
			if (isDoubleDir)
				b->m_Agent->m_TotalPayoff += R;
		}
		else if (a->m_Agent->m_Strategy == 0 && b->m_Agent->m_Strategy == 1)
		{
			a->m_Agent->m_TotalPayoff += S;
			if (isDoubleDir)
				b->m_Agent->m_TotalPayoff += T;
		}
		else if (a->m_Agent->m_Strategy == 1 && b->m_Agent->m_Strategy == 0)
		{
			a->m_Agent->m_TotalPayoff += T;
			if (isDoubleDir)
				b->m_Agent->m_TotalPayoff += S;
		}
		else if (a->m_Agent->m_Strategy == 1 && b->m_Agent->m_Strategy == 1)
		{
			a->m_Agent->m_TotalPayoff += P;
			if (isDoubleDir)
				b->m_Agent->m_TotalPayoff += P;
		}
	}

	// Evolve Strategy ////
	void fn_Evolve(Li_Node *a)
	{
		// randomly take a neighbor, if it have better payoff, take its strategy by a probability
		/*
		int tmp = rand()&7;

		if (a.mConn[tmp]->mTotalPayoff > a.mTotalPayoff)
		{
			double rate = a.mConn[tmp]->mTotalPayoff - a.mTotalPayoff;
			double tp = (rand() % int(T*100*8)) / 100.0 ;
			if (tp < rate)
				a.mUpdateStrategy = a.mConn[tmp]->mStrategy;
		}
		*/

		// Highest Payoff neighbor
		//STRATEGY tmpS = a.mStrategy;
		double tmpP = a->m_Agent->m_AveragePayoff;
		int tmpOrder = -1;

		int tsize = a->m_Conn.size();
		for (int i = 0; i < tsize; i++)
		{
			if (tmpP < a->m_Conn[i]->m_Agent->m_AveragePayoff)
			{
				//tmpS = a.mConn[i]->mStrategy;
				tmpP = a->m_Conn[i]->m_Agent->m_AveragePayoff;
				tmpOrder = i;
			}
			/*
			// 相同payoff的处理方式
			else if (tmpP == a.mConn[i]->mTotalPayoff)
			{
				int tr = rand()&10;
				if (tr < 5)
				{
					//tmpS = a.mConn[i]->mStrategy;
					tmpP = a.mConn[i]->mTotalPayoff;
					tmpOrder = i;
				}
				// end if
			}
			*/
			// end if-else
		}
		// end for

		/*
		if (ITERATE)
		{
			// for iterate game, copy the iterate strategy, and calculate the observe strategy
			if (tmpOrder != -1)
				a.mUpdateIStrategy = a.mConn[tmpOrder]->mIStrategy;
			else
				a.mUpdateIStrategy = a.mIStrategy;

			switch(a.mUpdateIStrategy)
			{
			case PUREC:
				{
					a.mUpdateStrategy = C;
				} break;
			case PURED:
				{
					a.mUpdateStrategy = D;
				} break;
			case TFT:
				{
					a.mUpdateStrategy = C;
				} break;
			}
			// end switch
		}
		else
		*/
		{
			// for non iterate game ,only copy observe strategy
			if (tmpOrder != -1)
				a->m_Agent->m_UpdateStrategy = a->m_Conn[tmpOrder]->m_Agent->m_Strategy;
			else
				a->m_Agent->m_UpdateStrategy = a->m_Agent->m_Strategy;
			//a.mUpdateStrategy = tmpS;
		}

		// Cooperate probability is total payoff of cooperate neighbor over all neighbor
		/*
		double cp = 0;
		double tp = 0;

		tp += a.mTotalPayoff;
		if (a.mStrategy == 0)
			cp += a.mTotalPayoff;

		for (int i = 0; i < 8; i++)
		{
			tp += a.mConn[i]->mTotalPayoff;
			if (a.mConn[i]->mStrategy == 0)
				cp += a.mConn[i]->mTotalPayoff;
		}

		if (tp == 0)
			tp = 1;

		long tmp = rand() % int(tp*100);
		if (tmp < long(cp*100))
			a.mUpdateStrategy = 0;
		else
			a.mUpdateStrategy = 1;
		*/
	}

	// iterated strategy
	/*
	void iterate(Player &a)
	{
		//int tmpS = a.mStrategy;
		if (a.mIStrategy == TFT) // for a TFT player
		{
			//count cooperate neighbours
			int num = 0;
			int tsize = a.mConn.size();
			for (int i = 0; i < tsize; i++)
			{
				if (a.mConn[i]->mStrategy == 0)	// if strategy is cooperate
					num++;
				// end if
			}
			// end for
			if (num >= TFTTRACE)
				a.mUpdateStrategy = C;				// the new strategy is cooperate
			else
				a.mUpdateStrategy = D;				// the new strategy is defact
		}
		// end if
	}
	// end function
	*/
};
// end class

#endif