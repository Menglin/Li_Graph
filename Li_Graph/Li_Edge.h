#ifndef LI_EDGE_H
#define LI_EDGE_H

class Li_Edge
{
public:
	int VertexS;
	int VertexE;

	Li_Edge(int VS, int VE) : VertexS(VS), VertexE(VE) {}
};

#endif