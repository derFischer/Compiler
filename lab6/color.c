#include <stdio.h>
#include <string.h>

#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "absyn.h"
#include "assem.h"
#include "frame.h"
#include "graph.h"
#include "liveness.h"
#include "color.h"
#include "table.h"
static G_nodeList simplifyWorklist = NULL;
static G_nodeList freezeWorklist = NULL;
static G_nodeList spillWorklist = NULL;

static G_nodeList spilledNodes = NULL;
static G_nodeList coalescedNodes = NULL;
static G_nodeList coloredNodes = NULL;

static G_nodeList selectStack = NULL;

static Live_moveList coalescedMoves = NULL;
static Live_moveList constrainedMoves = NULL;
static Live_moveList frozenMoves = NULL;
static Live_moveList worklistMoves = NULL;
static Live_moveList activeMoves = NULL;

const regNum = 16;

static G_nodeList Adjacent(G_node node)
{
	return setMinus(G_adj(node), setUnion(selectStack, coalescedNodes));
}

static bool MoveRelated(G_node node)
{
	return inMoveList()
}

void MakeWorklist(G_graph ig)
{
	
	G_nodeList nodelist = G_Nodes(ig);
	for(; nodelist != NULL; nodelist = nodelist->tail)
	{
		nodeInfo info = G_nodeInfo(nodelist->head);
		if(info->degree >= regNum)
		{
			spillWorklist = G_NodeList(nodelist->head, spillWorklist);
		}
		else if()
	}
}

void Simplify()
{

}

struct COL_result COL_color(G_graph ig, Temp_map initial, Temp_tempList regs, Live_moveList moves) {
	//your code here.
	while(simplifyWorklist != NULL)
	{
		
	}
	struct COL_result ret;
	return ret;
}