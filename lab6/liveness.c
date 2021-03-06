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
#include "color.h"

nodeInfo NodeInfo(Temp_temp reg)
{
	nodeInfo info = malloc(sizeof(*info));
	info->reg = reg;
	info->degree = 0;
	info->alias = NULL;
	info->moves = NULL;
	return info;
}

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail)
{
	Live_moveList lm = (Live_moveList)checked_malloc(sizeof(*lm));
	lm->src = src;
	lm->dst = dst;
	lm->tail = tail;
	return lm;
}

//helper
bool framePointer(Temp_temp reg)
{
	return (reg == F_RSP() || reg == F_RBP());
}

bool L_inMoveList(G_node src, G_node dst, Live_moveList moveList)
{
	for (; moveList != NULL; moveList = moveList->tail)
	{
		if ((src == moveList->src && dst == moveList->dst) || (src == moveList->dst && dst == moveList->src))
		{
			return TRUE;
		}
	}
	return FALSE;
}

Live_moveList L_setMinus(Live_moveList ml1, Live_moveList ml2)
{
	Live_moveList result = NULL;
	for (; ml1 != NULL; ml1 = ml1->tail)
	{
		if (L_inMoveList(ml1->src, ml1->dst, ml2))
		{
			continue;
		}
		result = Live_MoveList(ml1->src, ml1->dst, result);
	}
	return result;
}

Live_moveList L_setUnion(Live_moveList ml1, Live_moveList ml2)
{
	Live_moveList result = NULL;
	for (; ml1 != NULL; ml1 = ml1->tail)
	{
		result = Live_MoveList(ml1->src, ml1->dst, result);
	}
	for (; ml2 != NULL; ml2 = ml2->tail)
	{
		if (!L_inMoveList(ml2->src, ml2->dst, result))
		{
			result = Live_MoveList(ml2->src, ml2->dst, result);
		}
	}
	return result;
}

bool L_inTempList(Temp_temp tmp, Temp_tempList tmpl)
{
	while (tmpl)
	{
		if (tmpl->head == tmp)
		{
			return TRUE;
		}
		tmpl = tmpl->tail;
	}
	return FALSE;
}

Temp_tempList L_tempListMinus(Temp_tempList tmpl1, Temp_tempList tmpl2)
{
	Temp_tempList result = NULL;
	for (; tmpl1 != NULL; tmpl1 = tmpl1->tail)
	{
		if (L_inTempList(tmpl1->head, tmpl2))
		{
			continue;
		}
		result = Temp_TempList(tmpl1->head, result);
	}
	return result;
}

Temp_tempList L_tempListUnion(Temp_tempList tmpl1, Temp_tempList tmpl2)
{
	Temp_tempList result = NULL;
	for (; tmpl1 != NULL; tmpl1 = tmpl1->tail)
	{
		result = Temp_TempList(tmpl1->head, result);
	}
	for (; tmpl2 != NULL; tmpl2 = tmpl2->tail)
	{
		if (L_inTempList(tmpl2->head, result))
		{
			continue;
		}
		result = Temp_TempList(tmpl2->head, result);
	}
	return result;
}

bool sameTempList(Temp_tempList tmpl1, Temp_tempList tmpl2)
{
	Temp_tempList tmpl = tmpl1;
	for (; tmpl != NULL; tmpl = tmpl->tail)
	{
		if (!L_inTempList(tmpl->head, tmpl2))
		{
			return FALSE;
		}
	}
	for (; tmpl2 != NULL; tmpl2 = tmpl2->tail)
	{
		if (!L_inTempList(tmpl2->head, tmpl1))
		{
			return FALSE;
		}
	}
	return TRUE;
}

Temp_tempList L_calSuccIn(G_node node, TAB_table tempListIn)
{
	Temp_tempList result = NULL;
	G_nodeList succ = G_succ(node);
	for (; succ != NULL; succ = succ->tail)
	{
		G_node n = succ->head;
		result = L_tempListUnion(TAB_look(tempListIn, n), result);
	}
	return result;
}

bool isHardwareNode(G_node node)
{
	Temp_temp nodeTemp = G_getReg(node);
	string n = Temp_look(F_tempMap, nodeTemp);
	return (n != NULL);
}

struct Live_graph Live_liveness(G_graph flow)
{
	//printFlowgraph(flow);
	//your code here.
	struct Live_graph lg;

	//analysis
	G_nodeList nodes;
	TAB_table tempListIn = TAB_empty();
	TAB_table tempListOut = TAB_empty();
	bool fixPoint = FALSE;
	while (!fixPoint)
	{
		nodes = G_nodes(flow);
		fixPoint = TRUE;
		while (nodes)
		{
			G_node node = nodes->head;
			Temp_tempList oldIn = TAB_look(tempListIn, node);
			Temp_tempList oldOut = TAB_look(tempListOut, node);
			FG_use(node);
			FG_def(node);
			Temp_tempList newIn = L_tempListUnion(FG_use(node), L_tempListMinus(oldOut, FG_def(node)));
			Temp_tempList newOut = L_calSuccIn(node, tempListIn);
			if (!sameTempList(oldIn, newIn) || !sameTempList(oldOut, newOut))
			{
				fixPoint = FALSE;
			}
			TAB_enter(tempListIn, node, newIn);
			TAB_enter(tempListOut, node, newOut);
			nodes = nodes->tail;
		}
	}

	G_graph interference = G_Graph();
	TAB_table tempToNode = TAB_empty();
	G_nodeList allInstr = G_nodes(flow);

	while (allInstr)
	{
		Temp_tempList defs = FG_def(allInstr->head);
		Temp_tempList uses = FG_use(allInstr->head);
		while (defs)
		{
			if (TAB_look(tempToNode, defs->head) == NULL)
			{
				G_node tempNode = G_Node(interference, NodeInfo(defs->head));
				TAB_enter(tempToNode, defs->head, tempNode);
			}
			defs = defs->tail;
		}
		while (uses)
		{
			if (TAB_look(tempToNode, uses->head) == NULL)
			{
				G_node tempNode = G_Node(interference, NodeInfo(uses->head));
				TAB_enter(tempToNode, uses->head, tempNode);
			}
			uses = uses->tail;
		}
		allInstr = allInstr->tail;
	}

	nodes = G_nodes(flow);
	while (nodes)
	{
		G_node node = nodes->head;
		if (!FG_isMove(node))
		{
			Temp_tempList defs = FG_def(node);
			while (defs)
			{
				Temp_temp defTemp = defs->head;
				Temp_tempList outs = TAB_look(tempListOut, node);
				G_node defTempNode = TAB_look(tempToNode, defTemp);
				while (outs)
				{
					Temp_temp outTemp = outs->head;
					G_node outTempNode = TAB_look(tempToNode, outTemp);
					G_addEdgeNonDirection(defTempNode, outTempNode);
					outs = outs->tail;
				}
				defs = defs->tail;
			}
		}
		else
		{
			Temp_tempList moveDst = FG_MoveDst(node);
			Temp_tempList moveSrc = FG_MoveSrc(node);
			while (moveDst)
			{
				Temp_temp movTemp = moveDst->head;
				G_node movTempNode = TAB_look(tempToNode, movTemp);
				Temp_tempList outs = TAB_look(tempListOut, node);
				while (outs)
				{
					Temp_temp outTemp = outs->head;
					if (L_inTempList(outTemp, moveSrc))
					{
						outs = outs->tail;
						continue;
					}
					G_node outTempNode = TAB_look(tempToNode, outTemp);
					G_addEdgeNonDirection(movTempNode, outTempNode);
					outs = outs->tail;
				}
				moveDst = moveDst->tail;
			}
		}
		nodes = nodes->tail;
	}

	G_nodeList interferenceNodes = G_nodes(interference);
	G_nodeList precoloredNodes = NULL;
	while(interferenceNodes)
	{
		G_node node = interferenceNodes->head;
		if(isHardwareNode(node))
		{
			precoloredNodes = G_NodeList(node, precoloredNodes);
		}
		interferenceNodes = interferenceNodes->tail;
	}

	while(precoloredNodes)
	{
		G_node head = precoloredNodes->head;
		G_nodeList tail = precoloredNodes->tail;
		while(tail)
		{
			G_addEdgeNonDirection(head, tail->head);
			tail = tail->tail;
		}
		precoloredNodes = precoloredNodes->tail;
	}

	G_nodeList inodes = G_nodes(interference);
	while (inodes)
	{
		G_node node = inodes->head;
		G_nodeList ajdNodes = G_adj(node);
		while (ajdNodes)
		{
			G_node node = ajdNodes->head;
			ajdNodes = ajdNodes->tail;
		}
		inodes = inodes->tail;
	}

	//movelist
	Live_moveList moves = NULL;
	nodes = G_nodes(flow);
	while (nodes)
	{
		G_node node = nodes->head;
		if (FG_isMove(node))
		{
			AS_instr inst = G_nodeInfo(node);
			Temp_temp src = (inst->u.MOVE.src)->head;
			Temp_temp dst = (inst->u.MOVE.dst)->head;
			if (dst != src)
			{
				G_node srcNode = TAB_look(tempToNode, src);
				G_node dstNode = TAB_look(tempToNode, dst);
				if (!L_inMoveList(srcNode, dstNode, moves))
				{
					moves = Live_MoveList(srcNode, dstNode, moves);
				}
			}
		}
		nodes = nodes->tail;
	}

	lg.graph = interference;
	lg.moves = moves;
	return lg;
}