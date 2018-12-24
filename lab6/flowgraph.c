#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "absyn.h"
#include "assem.h"
#include "frame.h"
#include "graph.h"
#include "flowgraph.h"
#include "errormsg.h"
#include "table.h"

typedef struct waitStruct_ *waitStruct;
void printTemp2(Temp_temp temp)
{
	Temp_map map = Temp_layerMap(F_tempMap, Temp_name());
	printf("temp %s\n", Temp_look(map, temp));
	return;
}
struct waitStruct_
{
	AS_targets waitsLabel;
	G_node node;
};
waitStruct WaitStruct(AS_targets waitsLabel, G_node node)
{
	waitStruct tmp = malloc(sizeof(*tmp));
	tmp->waitsLabel = waitsLabel;
	tmp->node = node;
	return tmp;
}

typedef struct waitStructList_ *waitStructList;
struct waitStructList_
{
	waitStruct head;
	waitStructList tail;
};

waitStructList WaitStructList(waitStruct head, waitStructList tail)
{
	waitStructList tmp = malloc(sizeof(*tmp));
	tmp->head = head;
	tmp->tail = tail;
	return tmp;
}

Temp_tempList FG_def(G_node n)
{
	//your code here.
	AS_instr inst = G_nodeInfo(n);
	switch (inst->kind)
	{
	case I_OPER:
	{
		return inst->u.OPER.dst;
	}
	case I_LABEL:
	{
		assert(0);
		return NULL;
	}
	case I_MOVE:
	{
		return inst->u.MOVE.dst;
	}
	default:
	{
		printf("incorrect kind of inst, please debug\n");
		assert(0);
		return NULL;
	}
	}
	return NULL;
}

Temp_tempList FG_use(G_node n)
{
	//your code here.
	AS_instr inst = G_nodeInfo(n);
	switch (inst->kind)
	{
	case I_OPER:
	{
		return inst->u.OPER.src;
	}
	case I_LABEL:
	{
		assert(0);
		return NULL;
	}
	case I_MOVE:
	{
		return inst->u.MOVE.src;
	}
	default:
	{
		printf("incorrect kind of inst, please debug\n");
		assert(0);
		return NULL;
	}
	}
	return NULL;
}

Temp_tempList FG_MoveDst(G_node n)
{
	AS_instr inst = G_nodeInfo(n);
	return inst->u.MOVE.dst;
}

Temp_tempList FG_MoveSrc(G_node n)
{
	AS_instr inst = G_nodeInfo(n);
	return inst->u.MOVE.src;
}

bool FG_isMove(G_node n)
{
	//your code here.
	AS_instr inst = G_nodeInfo(n);
	return inst->kind == I_MOVE;
}

void fillLabelNode(TAB_table nodeLabel, Temp_labelList labelList, G_node node)
{
	while (labelList)
	{
		TAB_enter(nodeLabel, labelList->head, node);
		labelList = labelList->tail;
	}
	return;
}

G_graph FG_AssemFlowGraph(AS_instrList il, F_frame f)
{
	//your code here.
	G_graph graph = G_Graph();
	TAB_table nodeLabel = TAB_empty();
	G_node prevNode = NULL;
	Temp_labelList labelList = NULL;
	waitStructList waitToFill = NULL;

	while (il)
	{
		AS_instr inst = il->head;
		switch (inst->kind)
		{
		case I_OPER:
		{
			G_node node = G_Node(graph, inst);
			fillLabelNode(nodeLabel, labelList, node);
			if (prevNode)
			{
				G_addEdge(prevNode, node);
			}
			prevNode = node;
			if (strncmp(inst->u.OPER.assem, "jmp", 3) == 0)
			{
				prevNode = NULL;
			}
			if (inst->u.OPER.jumps)
			{
				waitToFill = WaitStructList(WaitStruct(inst->u.OPER.jumps, node), waitToFill);
			}
			break;
		}
		case I_LABEL:
		{
			labelList = Temp_LabelList(inst->u.LABEL.label, NULL);
			break;
		}
		case I_MOVE:
		{
			if (strcmp(inst->u.MOVE.assem, "movq `s0, `d0") == 0)
			{
				if ((inst->u.MOVE.dst)->head == (inst->u.MOVE.src)->head)
				{
					break;
				}
			}
			G_node node = G_Node(graph, inst);
			fillLabelNode(nodeLabel, labelList, node);
			if (prevNode)
			{
				G_addEdge(prevNode, node);
			}
			prevNode = node;
			break;
		}
		}
		il = il->tail;
	}

	while (waitToFill)
	{
		waitStruct tmp = waitToFill->head;
		G_node node = tmp->node;
		Temp_labelList labels = tmp->waitsLabel->labels;
		while (labels)
		{
			G_addEdge(node, TAB_look(nodeLabel, labels->head));
			labels = labels->tail;
		}
		waitToFill = waitToFill->tail;
	}
	return graph;
}
