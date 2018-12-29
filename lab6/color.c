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

bool adjOk(G_node center, G_node src);
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

const int regNum = 14;
const int MAX = 10000000;
void printTemp233(Temp_temp temp);
void printGnodeList(G_nodeList nodelist);

bool isPrecoloredNodes(G_node node)
{
	Temp_temp nodeTemp = G_getReg(node);
	string n = Temp_look(F_tempMap, nodeTemp);
	return n;
}

static G_nodeList Adjacent(G_node node)
{
	return G_setMinus(G_adj(node), G_setUnion(selectStack, coalescedNodes));
}

/*static Live_moveList NodeMoves(G_node node)
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
}*/

static Live_moveList NodeMoves(G_node node)
{
	Live_moveList result = NULL;
	nodeInfo info = G_nodeInfo(node);
	Live_moveList effectiveMoves = L_setUnion(activeMoves, worklistMoves);
	Live_moveList nodeMoves = info->moves;
	while (nodeMoves)
	{
		if (L_inMoveList(nodeMoves->src, nodeMoves->dst, effectiveMoves))
		{
			result = Live_MoveList(nodeMoves->src, nodeMoves->dst, result);
		}
		nodeMoves = nodeMoves->tail;
	}
	return result;
}

static bool MoveRelated(G_node node)
{
	return (NodeMoves(node) != NULL);
}

void MakeWorklist(G_graph ig)
{
	G_nodeList nodelist = G_nodes(ig);
	for (; nodelist != NULL; nodelist = nodelist->tail)
	{
		G_node node = nodelist->head;
		nodeInfo info = G_nodeInfo(node);
		if (G_inNodeList(node, precoloredList))
		{
			continue;
		}
		else if (info->degree >= regNum)
		{
			spillWorklist = G_NodeList(node, spillWorklist);
		}
		else if (MoveRelated(node))
		{
			printf("-------------============move related nodex============------------\n");
			printTemp233(G_getReg(node));
			printf("\n");
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
		Live_moveList nodemoves = NodeMoves(nodes->head);
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
	if (info->degree == regNum)
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
	G_node node = simplifyWorklist->head;
	simplifyWorklist = simplifyWorklist->tail;
	selectStack = G_NodeList(node, selectStack);
	G_nodeList adj = G_adj(node);
	printf("---------------------------simplify--------------------------\n");
	printTemp233(G_getReg(node));
	printf("\n");
	for (; adj != NULL; adj = adj->tail)
	{
		DecrementDegree(adj->head);
	}
}

void AddWorkList(G_node node)
{
	printf("--------------------add worklist-----------------\n");
	nodeInfo info = G_nodeInfo(node);
	if (!G_inNodeList(node, precoloredList) && !MoveRelated(node) && info->degree < regNum)
	{
		freezeWorklist = G_setMinus(freezeWorklist, G_NodeList(node, NULL));
		simplifyWorklist = G_setUnion(simplifyWorklist, G_NodeList(node, NULL));
	}
	printf("-------------------end add worklist--------------------\n");
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
	for (; nl != NULL; nl = nl->tail)
	{
		if (!OK(nl->head, src))
		{
			return FALSE;
		}
	}
	return TRUE;
}

bool Conservative(G_nodeList nodes)
{
	int highDegreeNum = 0;
	for (; nodes != NULL; nodes = nodes->tail)
	{
		nodeInfo info = G_nodeInfo(nodes->head);
		if (info->degree >= regNum)
		{
			highDegreeNum++;
		}
	}
	return (highDegreeNum < regNum);
}

void Combine(G_node node1, G_node node2)
{
	printf("--------------================Combine=========------------\n");
	printTemp233(G_getReg(node1));
	printTemp233(G_getReg(node2));
	printf("\n");
	if (G_inNodeList(node2, freezeWorklist))
	{
		freezeWorklist = G_setMinus(freezeWorklist, G_NodeList(node2, NULL));
	}
	else
	{
		spillWorklist = G_setMinus(spillWorklist, G_NodeList(node2, NULL));
	}
	coalescedNodes = G_setUnion(coalescedNodes, G_NodeList(node2, NULL));
	nodeInfo info1 = G_nodeInfo(node1);
	nodeInfo info2 = G_nodeInfo(node2);
	info2->alias = node1;
	info1->moves = L_setUnion(info1->moves, info2->moves);
	EnableMoves(G_NodeList(node2, NULL));
	G_nodeList node2Adj = G_adj(node2);
	for (; node2Adj != NULL; node2Adj = node2Adj->tail)
	{
		G_addEdge(node2Adj->head, node1);
		DecrementDegree(node2Adj->head);
	}
	if (info1->degree >= regNum && G_inNodeList(node1, freezeWorklist))
	{
		freezeWorklist = G_setMinus(freezeWorklist, G_NodeList(node1, NULL));
		spillWorklist = G_setUnion(spilledNodes, G_NodeList(node1, NULL));
	}
	printf("------------------==============end combine==============--------------\n");
}

void Coalesce()
{
	printf("-------------------coalesce------------------\n");
	while (worklistMoves)
	{
		G_node rawSrc = worklistMoves->src;
		G_node rawDst = worklistMoves->dst;
		assert(rawDst);
		assert(rawSrc);
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
		assert(newSrc);
		assert(newDst);
		worklistMoves = worklistMoves->tail;
		if (newSrc == newDst)
		{
			coalescedMoves = Live_MoveList(rawSrc, rawDst, coalescedMoves);
			AddWorkList(newSrc);
		}
		else if (G_inNodeList(newDst, precoloredList) || G_inNodeList(newDst, G_adj(newSrc)))
		{
			constrainedMoves = Live_MoveList(rawSrc, rawDst, constrainedMoves);
			AddWorkList(newSrc);
			AddWorkList(newDst);
		}
		else if ((G_inNodeList(newSrc, precoloredList) && adjOk(newDst, newSrc)) || (!G_inNodeList(newSrc, precoloredList) && Conservative(G_setUnion(G_adj(newSrc), G_adj(newDst)))))
		{
			coalescedMoves = Live_MoveList(rawSrc, rawDst, coalescedMoves);
			Combine(newSrc, newDst);
			AddWorkList(newSrc);
		}
		else
		{
			activeMoves = Live_MoveList(rawSrc, rawDst, activeMoves);
		}
	}
	printf("---------------------end coalesce-------------------\n");
}

void FreezeMoves(G_node node)
{
	printf("enter freezmoves\n");
	Live_moveList ml;
	while (ml = NodeMoves(node))
	{
		printf("enter here\n");
		G_node tmp;
		if (GetAlias(ml->dst) == GetAlias(node))
		{
			tmp = GetAlias(ml->src);
		}
		else
		{
			tmp = GetAlias(ml->dst);
		}
		printf("enter hereeeeee\n");
		activeMoves = L_setMinus(activeMoves, Live_MoveList(ml->src, ml->dst, NULL));
		frozenMoves = Live_MoveList(ml->src, ml->dst, frozenMoves);
		nodeInfo tmpInfo = G_nodeInfo(tmp);
		if (NodeMoves(tmp) == NULL && tmpInfo->degree < regNum)
		{
			freezeWorklist = G_setMinus(freezeWorklist, G_NodeList(tmp, NULL));
			simplifyWorklist = G_setUnion(simplifyWorklist, G_NodeList(tmp, NULL));
		}
	}
	printf("end freezmoves\n");
}

void Freeze()
{
	while (freezeWorklist)
	{
		G_node node = freezeWorklist->head;
		freezeWorklist = G_setMinus(freezeWorklist, G_NodeList(node, NULL));
		simplifyWorklist = G_setUnion(simplifyWorklist, G_NodeList(node, NULL));
		FreezeMoves(node);
	}
}

void SelectSpill()
{
	G_node node = spillWorklist->head;
	spilledNodes = spillWorklist->tail;
	simplifyWorklist = G_setUnion(simplifyWorklist, G_NodeList(node, NULL));
	FreezeMoves(node);
}

void AssignColors(Temp_map colored)
{
	while (selectStack)
	{
		G_node node = selectStack->head;
		selectStack = selectStack->tail;
		Temp_tempList colors = F_allRegisters();
		G_nodeList adjNodes = G_adj(node);
		if (G_inNodeList(node, precoloredList))
		{
			continue;
		}
		while (adjNodes)
		{
			G_node adjNode = adjNodes->head;
			if (G_inNodeList(GetAlias(adjNode), coloredNodes) || G_inNodeList(GetAlias(adjNode), precoloredList))
			{
				nodeInfo info = G_nodeInfo(GetAlias(adjNode));
				string reg = Temp_look(colored, info->reg);
				assert(reg != NULL);
				Temp_temp color = Temp_stringToTemp(reg);
				colors = L_tempListMinus(colors, Temp_TempList(color, NULL));
			}
			adjNodes = adjNodes->tail;
		}
		if (colors == NULL)
		{
			spilledNodes = G_NodeList(node, spilledNodes);
		}
		else
		{
			nodeInfo info = G_nodeInfo(node);
			coloredNodes = G_NodeList(node, coloredNodes);
			Temp_temp color = colors->head;
			assert(Temp_look(F_tempMap, color) != NULL);
			assert(info != NULL);
			assert(info->reg != NULL);
			printf("-----------------assign color--------------\n");
			printf("assign:");
			printTemp233(info->reg);
			printf("with color:");
			printTemp233(color);
			printf("\n");
			Temp_enter(colored, info->reg, Temp_look(F_tempMap, color));
		}
	}
	while (coalescedNodes)
	{
		G_node node = coalescedNodes->head;
		G_node alias = GetAlias(node);
		nodeInfo aliasInfo = G_nodeInfo(alias);
		nodeInfo info = G_nodeInfo(node);
		printf("-----------------assign color--------------\n");
		printf("assign:");
		printTemp233(info->reg);
		printf("with color:");
		printTemp233(color);
		printf("\n");
		Temp_enter(colored, info->reg, Temp_look(colored, aliasInfo->reg));
		coalescedNodes = coalescedNodes->tail;
	}
}

Live_moveList calculateMoves(G_node node, Live_moveList moves)
{
	Live_moveList result;
	while (moves)
	{
		if (moves->src == node || moves->dst == node)
		{
			result = Live_MoveList(moves->src, moves->dst, result);
		}
		moves = moves->tail;
	}
	return result;
}

void Build(G_graph ig, Live_moveList moves)
{
	worklistMoves = moves;
	printf("enter build\n");
	G_nodeList nodes = G_nodes(ig);
	printf("build nodes:");
	printGnodeList(nodes);
	while (nodes)
	{
		G_node node = nodes->head;
		nodeInfo info = G_nodeInfo(node);
		info->degree = G_degree(node);
		info->moves = calculateMoves(node, moves);
		printTemp233(info->reg);
		if (isPrecoloredNodes(node))
		{
			precoloredList = G_NodeList(node, precoloredList);
			info->degree = MAX;
		}
		nodes = nodes->tail;
	}
	printf("end build");
}

void printTemp233(Temp_temp temp)
{
	Temp_map map = Temp_layerMap(F_tempMap, Temp_name());
	assert(Temp_look(map, temp));
	printf("temp %s\t", Temp_look(map, temp));
	return;
}

void printGnodeList(G_nodeList nodelist)
{
	while (nodelist)
	{
		nodeInfo info = G_nodeInfo(nodelist->head);
		printTemp233(info->reg);
		nodelist = nodelist->tail;
	}
	printf("\n");
}
struct COL_result COL_color(G_graph ig, Temp_map initial, Temp_tempList regs, Live_moveList moves)
{
	struct COL_result ret;
	printf("print build\n");
	Build(ig, moves);
	printf("end build\n");
	printf("print MakeWorklist\n");
	MakeWorklist(ig);
	printf("end MakeWorklist\n");
	//your code here.
	while (simplifyWorklist != NULL || worklistMoves != NULL || freezeWorklist != NULL || spillWorklist != NULL)
	{
		if (simplifyWorklist != NULL)
		{
			printf("print simplify\n");
			Simplify();
			printf("end simplify\n");
			//printf("simplify node list:");
			//printGnodeList(simplifyWorklist);
		}
		else if (worklistMoves != NULL)
		{
			printf("print coalesce\n");
			Coalesce();
			printf("end coalesce\n");
		}
		else if (freezeWorklist != NULL)
		{
			printf("print freeze\n");
			Freeze();
			printf("end freeze\n");
			//printf("freeze node list:");
			//printGnodeList(freezeWorklist);
		}
		else if (spillWorklist != NULL)
		{
			printf("print selecespill\n");
			SelectSpill();
			printf("end selectspill\n");
			//printf("spillworklist node list:");
			//printGnodeList(spillWorklist);
		}
		//sleep(1);
	}
	ret.coloring = Temp_layerMap(F_tempMap, Temp_empty());
	printf("print AssignColors\n");
	AssignColors(ret.coloring);
	printf("end AssignColors\n");
	Temp_tempList spills = NULL;
	while (spilledNodes)
	{
		nodeInfo info = G_nodeInfo(spilledNodes->head);
		spills = Temp_TempList(info->reg, spills);
		spilledNodes = spilledNodes->tail;
	}
	ret.spills = spills;
	return ret;
}