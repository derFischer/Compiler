#ifndef LIVENESS_H
#define LIVENESS_H

typedef struct Live_moveList_ *Live_moveList;
struct Live_moveList_ {
	G_node src, dst;
	Live_moveList tail;
};

struct nodeInfo_ {
	Temp_temp reg;
	int degree;
	G_node alias;
	Live_moveList moves;
	Temp_tempList in;
	Temp_tempList out;
}

typedef nodeInfo_* nodeInfo;
nodeInfo NodeInfo(int degree, G_node alias, Live_moveList moves)
{
	nodeInfo info = malloc(sizeof(*info));
	info->degree = degree;
	info->alias = alias;
	info->moves = moves;
	return info;
}

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
#endif
