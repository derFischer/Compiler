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

//helper
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

bool L_inTempList(Temp_temp tmp, Temp_tempList tmpl)
{
	while(tmpl)
	{
		if(tmpl->head == tmp)
		{
			return TRUE;
		}
		tmpl = tmpl->tail;
	}
	return FALSE;
}

Temp_tempList L_tempListMinus(Temp_tempList tmpl1, Temp_tempList tmpl2)
{
	Temp_tempList result;
	for(; tmpl1 != NULL; tmpl1 = tmpl1->tail)
	{
		if(L_inTempList(tmpl1->head, tmpl2))
		{
			continue;
		}
		result = Temp_TempList(tmpl1->head, result);
	}
	return result;
}

Temp_tempList L_tempListUnion(Temp_tempList tmpl1, Temp_tempList tmpl2)
{
	Temp_tempList result;
	for(; tmpl1 != NULL; tmpl1 = tmpl1->tail)
	{
		result = Temp_TempList(tmpl->head, result);
	}
	for(; tmpl2 != NULL; tmpl2 = tmpl2->tail)
	{
		if(L_inTempList(tmpl2->head, result))
		{
			continue;
		}
		result = Temp_TempList(tmpl2->head, result);
	}
	return result;
}

bool sameTempList(Temp_tempList tmpl1, Temp_tempList tmpl2)
{
	for(; tmpl1 != NULL && tmp2 != NULL; tmpl1 = tmpl1->tail, tmpl2 = tmpl2->tail)
	{
		if(tmpl1->head != tmp2->head)
		{
			return FALSE;
		}
	}
	if(tmpl1 != NULL || tmpl2 != NULL)
	{
		return FALSE;
	}
	return TRUE;
}

static void enterLiveMap(G_table t, G_node flowNode, nodeInfo info)
{
	G_enter(t, flowNode, info);
}


Temp_temp Live_gtemp(G_node node) {
	//your code here.
	nodeInfo info = G_nodeInfo(node);
	return info->reg;
}

Temp_tempList L_calSuccIn(G_node node, TAB_table tempListIn)
{
	Temp_tempList result = NULL;
	G_nodeList succ = G_succ(node);
	for(; succ != NULL; succ = succ->tail)
	{
		G_node n = succ->head;
		nodeInfo info = G_nodeInfo(n);
		result = L_tempListUnion(TAB_look(tempListIn, n), result);
	}
	return result;
}

struct Live_graph Live_liveness(G_graph flow) {
	//your code here.
	struct Live_graph lg;

	//analysis
	G_nodeList nodes = G_nodes(flow);
	TAB_table tempListIn = TAB_empty();
	TAB_table tempListOut = TAB_empty();
	bool fixPoint = FALSE

	while(fixPoint)
	{
		fixPoint = TRUE;
		while(nodes)
		{
			G_node node = nodes->head
			Temp_tempList oldIn = TAB_look(tempListIn, node);
			Temp_tempList oldOut = TAB_look(tempListOut, node);
			Temp_tempList newIn = L_tempListUnion(FG_use(node), L_tempListMinus(oldOut, FG_def(node)));
			Temp_tempLIst newOut = L_calSuccIn(node);

			if(!sameTempList(oldIn, newIn) || !sameTempList(oldOut, newOut))
			{
				fixPoint = FALSE;
			}
			TAB_enter(tempListIn, node, newIn);
			TAB_enter(tempListOut, node, newOut);
		}
	}
	
	//interference graph
	G_graph interference = G_empty();
	while(nodes)
	{
		
	}

	return lg;
}