#include <time.h>
#include <fstream>
#include <string>
#include "../Li_Graph/Li_Graph.h"
#pragma comment(lib, "../debug/Li_Graph.lib")

void main()
{
	srand((unsigned int)time(NULL)); // randomize timer

	string str;
	int Nodes;
	double ExpCC, ExpError;

	ifstream fin;
	fin.open("GraphParam.ini", ios::in);
	fin>>str>>Nodes>>str>>ExpCC>>str>>ExpError;

	Li_Graph g;
	g.fn_GenerateGraph(Nodes, ExpCC, ExpError);
	g.fn_SaveGraph("graph1.txt");
}