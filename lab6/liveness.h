#ifndef LIVENESS_H
#define LIVENESS_H
#include "graph.h"

typedef struct Live_moveList_ *Live_moveList;
struct Live_moveList_ {
	G_node src, dst;
	Live_moveList tail;
};

typedef struct nodeInfo_* nodeInfo;
struct nodeInfo_ {
	Temp_temp reg;
	int degree;
	G_node alias;
	Live_moveList moves;
};

nodeInfo NodeInfo(Temp_temp reg);

/*nodeInfo NodeInfo(int degree, G_node alias, Live_moveList moves)
{
	nodeInfo info = malloc(sizeof(*info));
	info->degree = degree;
	info->alias = alias;
	info->moves = moves;
	return info;
}*/

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail);

struct Live_graph {
	G_graph graph;
	Live_moveList moves;
};
Temp_temp Live_gtemp(G_node n);

struct Live_graph Live_liveness(G_graph flow);

bool L_inMoveList(G_node src, G_node dst, Live_moveList moveList);
Live_moveList L_setMinus(Live_moveList ml1, Live_moveList ml2);
Live_moveList L_setUnion(Live_moveList ml1, Live_moveList ml2);
Temp_tempList L_tempListUnion(Temp_tempList tmpl1, Temp_tempList tmpl2);
Temp_tempList L_tempListMinus(Temp_tempList tmpl1, Temp_tempList tmpl2);
bool L_inTempList(Temp_temp tmp, Temp_tempList tmpl);
#endif
