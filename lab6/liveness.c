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

void printTemp(Temp_temp temp)
{
	Temp_map map = Temp_layerMap(F_tempMap, Temp_name());
	printf("temp %s\n", Temp_look(map, temp));
	return;
}

void printTemp3(Temp_temp temp)
{
	Temp_map map = Temp_layerMap(F_tempMap, Temp_name());
	printf("temp %s\t", Temp_look(map, temp));
	return;
}

void printTempList(Temp_tempList temp)
{
	while (temp)
	{
		printTemp3(temp->head);
		temp = temp->tail;
	}
	printf("\n");
}

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

Temp_temp Live_gtemp(G_node node)
{
	//your code here.
	nodeInfo info = G_nodeInfo(node);
	return info->reg;
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

void printFlowgraph(G_graph flow)
{
	G_nodeList nodes = G_nodes(flow);
	while (nodes)
	{
		G_node node = nodes->head;
		AS_instr inst = G_nodeInfo(node);
		printf("inst:");
		printf("%s\n", inst->u.MOVE.assem);
		G_nodeList succ = G_succ(node);
		printf("succ:");
		while (succ)
		{
			AS_instr inst = G_nodeInfo(succ->head);
			printf("%s\n", inst->u.MOVE.assem);
			succ = succ->tail;
		}
		nodes = nodes->tail;
	}
}

void printInst(G_node node)
{
	AS_print(stdout, G_getInstr(node), Temp_layerMap(F_tempMap, Temp_name()));
}

struct Live_graph Live_liveness(G_graph flow)
{
	printf("flow graph:");
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
		//printf("outer loop start\n");
		nodes = G_nodes(flow);
		fixPoint = TRUE;
		while (nodes)
		{
			//printf("inner loop start\n");
			G_node node = nodes->head;
			//AS_print(stdout, G_getInstr(node), Temp_layerMap(F_tempMap, Temp_name()));
			Temp_tempList oldIn = TAB_look(tempListIn, node);
			Temp_tempList oldOut = TAB_look(tempListOut, node);
			//printf("inner loop point 2\n");
			FG_use(node);
			//printf("inner loop point 4\n");
			FG_def(node);
			//printf("inner loop point 5\n");
			//showTempList(FG_use(node));
			//printf("inner loop point 6\n");
			//showTempList(FG_def(node));
			//printf("inner loop point 7\n");
			//showTempList(oldOut);
			Temp_tempList newIn = L_tempListUnion(FG_use(node), L_tempListMinus(oldOut, FG_def(node)));
			//printf("fix point loop point 3\n");
			Temp_tempList newOut = L_calSuccIn(node, tempListIn);
			//printf("fix point loop point 1\n");
			if (!sameTempList(oldIn, newIn) || !sameTempList(oldOut, newOut))
			{
				fixPoint = FALSE;
			}
			TAB_enter(tempListIn, node, newIn);
			TAB_enter(tempListOut, node, newOut);
			/****************************************/
			// free(oldIn);
			// free(oldOut);
			/****************************************/
			nodes = nodes->tail;
		}
	}

	//interference graph
	//to do:判断重复
	printf("start build interference\n");
	G_graph interference = G_Graph();
	//construct the temp to node table
	TAB_table tempToNode = TAB_empty();
	G_nodeList allInstr = G_nodes(flow);

	while (allInstr)
	{
		Temp_tempList defs = FG_def(allInstr->head);
		Temp_tempList uses = FG_use(allInstr->head);
		while (defs)
		{
			/*if(framePointer(defs->head))
			{
				defs = defs->tail;
				continue;
			}*/
			if (TAB_look(tempToNode, defs->head) == NULL)
			{
				G_node tempNode = G_Node(interference, NodeInfo(defs->head));
				TAB_enter(tempToNode, defs->head, tempNode);
			}
			defs = defs->tail;
		}
		while (uses)
		{
			/*if(framePointer(uses->head))
			{
				uses = uses->tail;
				continue;
			}*/
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
		printInst(node);
		if (!FG_isMove(node))
		{
			Temp_tempList defs = FG_def(node);
			while (defs)
			{
				Temp_temp defTemp = defs->head;
				Temp_tempList outs = TAB_look(tempListOut, node);
				/*if (framePointer(defTemp))
				{
					defs = defs->tail;
					continue;
				}
				/*if (TAB_look(tempToNode, defTemp) == NULL)
				{
					G_node defTempNode = G_Node(interference, NodeInfo(defTemp));
					printf("enter nonmove def temp:");
					printTemp(defTemp);
					TAB_enter(tempToNode, defTemp, defTempNode);
				}*/
				G_node defTempNode = TAB_look(tempToNode, defTemp);
				while (outs)
				{
					Temp_temp outTemp = outs->head;
					/*if (framePointer(outTemp))
					{
						outs = outs->tail;
						continue;
					}
					/*if (TAB_look(tempToNode, outTemp) == NULL)
					{
						G_node outTempNode = G_Node(interference, NodeInfo(outTemp));
						printf("enter nonmove out temp:");
						printTemp(outTemp);
						TAB_enter(tempToNode, outTemp, outTempNode);
					}*/
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
				/*if (framePointer(movTemp))
				{
					moveDst = moveDst->tail;
					continue;
				}
				/*if (TAB_look(tempToNode, movTemp) == NULL)
				{
					G_node movTempNode = G_Node(interference, NodeInfo(movTemp));
					printf("enter move use temp:");
					printTemp(movTemp);
					TAB_enter(tempToNode, movTemp, movTempNode);
				}*/
				G_node movTempNode = TAB_look(tempToNode, movTemp);
				Temp_tempList outs = TAB_look(tempListOut, node);
				while (outs)
				{
					Temp_temp outTemp = outs->head;
					/*if (framePointer(outTemp))
					{
						outs = outs->tail;
						continue;
					}*/
					if (L_inTempList(outTemp, moveSrc))
					{
						outs = outs->tail;
						continue;
					}
					/*if (TAB_look(tempToNode, outTemp) == NULL)
					{
						G_node outTempNode = G_Node(interference, NodeInfo(outTemp));
						printf("enter move out temp:");
						printTemp(outTemp);
						TAB_enter(tempToNode, outTemp, outTempNode);
					}*/
					G_node outTempNode = TAB_look(tempToNode, outTemp);
					G_addEdgeNonDirection(movTempNode, outTempNode);
					outs = outs->tail;
				}
				moveDst = moveDst->tail;
			}
		}
		nodes = nodes->tail;
	}

	printf("-------===========interference===========-------------\n");
	G_nodeList inodes = G_nodes(interference);
	while (inodes)
	{
		G_node node = inodes->head;
		printTemp3(G_getReg(node));
		printf("interference with:");
		G_nodeList ajdNodes = G_adj(node);
		while (ajdNodes)
		{
			G_node node = ajdNodes->head;
			printTemp3(G_getReg(node));
			ajdNodes = ajdNodes->tail;
		}
		printf("\n");
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
			/*if (framePointer(src) || framePointer(dst))
			{
				nodes = nodes->tail;
				continue;
			}*/
			assert(src != NULL);
			assert(dst != NULL);
			if (dst != src)
			{
				G_node srcNode = TAB_look(tempToNode, src);
				G_node dstNode = TAB_look(tempToNode, dst);
				if (!L_inMoveList(srcNode, dstNode, moves))
				{
					assert(srcNode);
					assert(dstNode);
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