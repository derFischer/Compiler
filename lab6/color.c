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

static G_nodeList precoloredList = NULL;
static G_nodeList initialList = NULL;
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
	for (; activeMoves != NULL; activeMoves = activeMoves->tail)
	{
		if (activeMoves->src == node || activeMoves->dst == node)
		{
			result = Live_MoveList(activeMoves->src, activeMoves->dst, result);
		}
	}
	for (; worklistMoves != NULL; worklistMoves = worklistMoves->tail)
	{
		if (worklistMoves->src == node || worklistMoves->dst == node)
		{
			if (!L_inMoveList(worklistMoves->src, worklistMoves->dst, result))
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
	for (; nodelist != NULL; nodelist = nodelist->tail)
	{
		G_node node = nodelist->head;
		nodeInfo info = G_nodeInfo(node);
		if (info->degree >= regNum)
		{
			spillWorklist = G_NodeList(node, spillWorklist);
		}
		else if (MoveRelated(node))
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
	for (; nodes != NULL; nodes = nodes->tail)
	{
		Live_moveList nodemoves = G_NodeMoves(nodes->head);
		for (; nodemoves != NULL; nodemoves = nodemoves->tail)
		{
			if (L_inMoveList(nodemoves->src, nodemoves->dst, activeMoves))
			{
				activeMoves = L_setMinus(activeMoves, Live_MoveList(nodemoves->src, nodemoves->dst, NULL));
				worklistMoves = Live_MoveList(nodemoves->src, nodemoves->dst, worklistMoves);
			}
		}
	}
}

void DecrementDegree(G_node node)
{
	nodeInfo info = G_nodeInfo(node);
	info->degree--;
	if (info->degree == K)
	{
		EnableMoves(G_setUnion(G_NodeList(node, NULL), G_adj(node)));
		spillWorklist = G_setMinus(spillWorklist, G_NodeList(node, NULL));
		if (MoveRelated(node))
		{
			freezeWorklist = G_setUnion(freezeWorklist, G_NodeList(node, NULL));
		}
		else
		{
			simplifyWorklist = G_setUnion(simplifyWorklist, G_NodeList(node, NULL));
		}
	}
}
void Simplify()
{
	while (simplifyWorklist)
	{
		G_node node = simplifyWorklist->head;
		simplifyWorklist = simplifyWorklist->tail;
		selectStack = G_NodeList(node, selectStack);
		G_nodeList adj = G_adj(node);
		for (; adj != NULL; adj = adj->tail)
		{
			DecrementDegree(adj->head);
		}
	}
}

void AddWorkList(G_node node)
{
	nodeInfo = G_nodeInfo(node);
	if(!G_inNodeList(node, precoloredList) && !MoveRelated(node) && nodeInfo->degree < regNum)
	{
		freezeWorklist = G_setMinus(freezeWorklist, G_NodeList(node, NULL));
		simplifyWorklist = G_setUnion(simplifyWorklist, G_NodeList(node, NULL));
	}
}

G_node GetAlias(G_node node)
{
	if (G_inNodeList(node, coalescedNodes))
	{
		nodeInfo info = G_nodeInfo(node);
		return GetAlias(info->alias);
	}
	else
	{
		return node;
	}
}

bool OK(G_node node1, G_node node2)
{
	nodeInfo info = G_nodeInfo(node1);
	bool degree = (info->degree < regNum);
	bool precolored = G_inNodeList(node1, precoloredList);
	bool adj = G_inNodeList(node2, G_adj(node1));
	return degree || precolored || adj;
}

bool adjOk(G_node center, G_node src)
{
	G_nodeList nl = G_adj(center);
	for(; nl != NULL; nl = nl->tail)
	{
		if(!OK(nl->head, src))
		{
			return false;
		}
	}
	return true;
}

bool Conservative(G_nodeList nodes)
{
	int highDegreeNum = 0;
	for(; nodes != NULL; nodes = nodes->tail)
	{
		nodeInfo = G_nodeInfo(nodes->head);
		if(nodeInfo->degree >= regNum)
		{
			highDegreeNum++;
		}
	}
	return (highDegreeNum < regNum);
}

void Combine(G_node node1, G_node node2)
{
	if(G_inNodeList(node2, freezeWorklist))
	{
		freezeWorklist = G_setMinus(freezeWorklist, G_NodeList(node2, NULL));
	}
	else
	{
		spillWorklist = G_setMinus(spillWorklist, G_NodeList(node2, NULL));
	}
	coalescedNodes = G_setUnion(coalescedNodes, G_NodeList(node2, NULL));
	G_nodeInfo(node2)->alias = u;
	
}

void Coalesce()
{
	while (worklistMoves)
	{
		G_node rawSrc = worklistMoves->src;
		G_node rawDst = worklistMoves->dst;
		G_node src = GetAlias(rawSrc);
		G_node dst = GetAlias(rawDst);
		G_node newSrc, newDst;
		if (G_inNodeList(dst, precoloredList))
		{
			newSrc = dst;
			newDst = src;
		}
		else
		{
			newSrc = src;
			newDst = dst;
		}
		worklistMoves = worklistMoves->tail;
		if(newSrc = newDst)
		{
			coalescedMoves = Live_MoveList(rawSrc, rawDst, coalescedMoves);
			AddWorkList(newSrc);
		}
		else if(G_inNodeList(newDst, precoloredList) || G_inNodeList(newDst, G_adj(newSrc)))
		{
			constrainedMoves = Live_MoveList(rawSrc, rawDst, constrainedMoves);
			AddWorkList(newSrc);
			AddWorkList(newDst);
		}
		else if(G_inNodeList(newSrc, precoloredList) && adjOK(newDst, newSrc) || !G_inNodeList(newSrc, precoloredList) && Conservative(G_setUnion(G_adj(newSrc), G_adj(newDst))))
		{
			coalescedMoves = Live_MoveList(rawSrc, rawDst, coalescedMoves);
			Combine()
		}
	}
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
	while (simplifyWorklist != NULL && worklistMoves != NULL && freezeWorklist != NULL & spillWorklist != NULL)
	{
		if (simplifyWorklist != NULL)
		{
			Simplify();
		}
		else if (worklistMoves != NULL)
		{
			Coalesce();
		}
		else if (freezeWorklist != NULL)
		{
			Freeze();
		}
		else if (spillWorklist != NULL)
		{
			SelectSpill();
		}
	}
	AssignColors();
	if (spilledNodes != NULL)
	{
		RewriteProgram(spilledNodes);
		COL_color(ig, initial, regs, moves);
	}
	struct COL_result ret;
	return ret;
}