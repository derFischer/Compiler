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
	return G_setMinus(G_adj(node), G_setUnion(selectStack, coalescedNodes));
}

static Live_moveList G_NodeMoves(G_node node)
{
	Live_moveList result = NULL;
	for(; activeMoves != NULL; activeMoves = activeMoves->tail)
	{
		if(activeMoves->src == node || activeMoves->dst == node)
		{
			result = Live_MoveList(activeMoves->src, activeMoves->dst, result);
		}
	}
	for(; worklistMoves != NULL; worklistMoves = worklistMoves->tail)
	{
		if(worklistMoves->src == node || worklistMoves->dst == node)
		{
			if(!L_inMoveList(worklistMoves->src, worklistMoves->dst, result))
			{
				result = Live_MoveList(worklistMoves->src, worklistMoves->dst, result);
			}
		}
	}
	return result;
}

static bool MoveRelated(G_node node)
{
	return !(G_NodeMoves(node));
}

void MakeWorklist(G_graph ig)
{	
	G_nodeList nodelist = G_nodes(ig);
	for(; nodelist != NULL; nodelist = nodelist->tail)
	{
		G_node node = nodelist->head;
		nodeInfo info = G_nodeInfo(node);
		if(info->degree >= regNum)
		{
			spillWorklist = G_NodeList(node, spillWorklist);
		}
		else if(MoveRelated(node))
		{
			freezeWorklist = G_NodeList(node, freezeWorklist);
		}
		else
		{
			simplifyWorklist = G_NodeList(node, simplifyWorklist);
		}
	}
}

void EnableMoves(G_nodeList nodes)
{
	for(; nodes != NULL; nodes = nodes->tail)
	{
		G_nodeList nodemoves = G_NodeMoves(nodes->head);
		for(; nodemoves != NULL; nodemoves = nodemoves->tail)
		{
			if()
		}
	}
}

void DecrementDegree(G_node node)
{
	nodeInfo info = G_nodeInfo(node);
	info->degree--;
	if(info->degree == K)
	{

	}
}
void Simplify()
{
	for(; simplifyWorklist != NULL; simplifyWorklist = simplifyWorklist->tail)
	{
		selectStack = G_NodeList(simplifyWorklist->head, selectStack);
		G_nodeList adj = G_adj(simplifyWorklist->head);
		for(; adj != NULL; adj = adj->tail)
		{

		}
	}

}

void Coalesce()
{

}

void Freeze()
{

}

void SelectSpill()
{

}

void AssignColors()
{

}

void RewriteProgram(G_nodeList spilledNodes)
{

}

struct COL_result COL_color(G_graph ig, Temp_map initial, Temp_tempList regs, Live_moveList moves) 
{
	MakeWorklist(ig);
	//your code here.
	while(simplifyWorklist != NULL && worklistMoves != NULL && freezeWorklist != NULL & spillWorklist != NULL)
	{
		if(simplifyWorklist != NULL)
		{
			Simplify();
		}
		else if(worklistMoves != NULL)
		{
			Coalesce();
		}
		else if(freezeWorklist != NULL)
		{
			Freeze();
		}
		else if(spillWorklist != NULL)
		{
			SelectSpill();
		}
	}
	AssignColors();
	if(spilledNodes != NULL)
	{
		RewriteProgram(spilledNodes);
		COL_color(ig, initial, regs, moves);
	}
	struct COL_result ret;
	return ret;
}