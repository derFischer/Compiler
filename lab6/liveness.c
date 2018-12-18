#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "absyn.h"
#include "assem.h"
#include "frame.h"
#include "graph.h"
#include "flowgraph.h"
#include "liveness.h"
#include "table.h"

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail) {
	Live_moveList lm = (Live_moveList) checked_malloc(sizeof(*lm));
	lm->src = src;
	lm->dst = dst;
	lm->tail = tail;
	return lm;
}


Temp_temp Live_gtemp(G_node n) {
	//your code here.
	return NULL;
}

struct Live_graph Live_liveness(G_graph flow) {
	//your code here.
	struct Live_graph lg;
	return lg;
}

bool L_inMoveList(G_node src, G_node dst, Live_moveList moveList)
{
	for(; moveList != NULL; moveList = moveList->tail)
	{
		if(src == moveList->src && dst == moveList->dst || src == moveList->dst && dst == moveList->src)
		{
			return TRUE;
		}
	}
	return FALSE;
}

Live_moveList L_setMinus(Live_moveList ml1, Live_moveList ml2)
{
	Live_moveList result;
	for(; ml1 != NULL; ml1 = ml1->tail)
	{
		if(L_inMoveList(ml1->src, ml1->dst, ml2))
		{
			continue;
		}
		result = Live_MoveList(ml1->src, ml1->dst, result);
	}
	return result;
}

Live_moveList L_setUnion(Live_moveList ml1, Live_moveList ml2)
{
	Live_moveList result;
	for(; ml1 != NULL; ml1 = ml1->tail)
	{
		result = Live_MoveList(ml1->src, ml1->dst, result);
	}
	for(; ml2 != NULL; ml2 = ml2->tail)
	{
		if(!L_inMoveList(ml2->src, ml2->dst, result))
		{
			result = Live_MoveList(ml2->src, ml2->dst, result);
		}
	}
	return result;
}