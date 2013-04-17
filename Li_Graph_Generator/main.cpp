#include <time.h>
#include <fstream>
#include <string>
#include "../Li_Graph/Li_Graph.h"
#include "stdlib.h"
#pragma comment(lib, "../debug/Li_Graph.lib")

void main()
{
	srand((unsigned int)time(NULL)); // randomize timer

	string str;
	int Nodes, ExpAvgDegree;
	double ExpCC, ExpError;

	ifstream fin;
	fin.open("GraphParam.ini", ios::in);
	fin>>str>>Nodes>>str>>ExpCC>>str>>ExpError>>str>>ExpAvgDegree;

	Li_Graph g;
	// g.fn_GenerateGraphASC(Nodes, ExpCC, ExpError, false);
	g.fn_GenerateGraph(Nodes, ExpCC, ExpError, ExpAvgDegree, true);
	g.fn_SaveGraph("newgraph.txt");

	//g.fn_GenerateGraphASCGroup(Nodes, ExpCC, ExpError, true);

	system("PAUSE");
}