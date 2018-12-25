#include <stdio.h>
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
#include "regalloc.h"
#include "table.h"
#include "flowgraph.h"
#include <string.h>

#define INSTLENGTH 200

Temp_tempList tempIntersection(Temp_tempList tmpl1, Temp_tempList tmpl2)
{
	Temp_tempList result = NULL;
	for(; tmpl1 != NULL; tmpl1 = tmpl1->tail)
	{
		if(L_inTempList(tmpl1->head, tmpl2))
		{
			result = Temp_TempList(tmpl1->head, result);
		}
	}
	return result;
}

AS_instrList RewriteProgram(F_frame f, AS_instrList il, Temp_tempList spills)
{
	TAB_table tempPos = TAB_empty();
	AS_instrList result = il;
	Temp_tempList spillAlloc = spills;

	//assign stack space for each spill temp
	while(spillAlloc)
	{
		TAB_enter(tempPos, spillAlloc->head, F_allocLocal(f, TRUE));
		spillAlloc = spillAlloc->tail;
	}

	//modify the instrs
	while(il)
	{
		AS_instr inst = il->head;
		Temp_tempList src = NULL;
		Temp_tempList dst = NULL;
		if(inst->kind == I_OPER)
		{
			src = inst->u.OPER.src;
			dst = inst->u.OPER.dst;
		}
		if(inst->kind == I_MOVE)
		{
			src = inst->u.MOVE.src;
			src = inst->u.MOVE.dst;
		}

		//calculate the temps that need to load/store of each instr
		Temp_tempList spilledSrc = tempIntersection(src, spills);
		Temp_tempList spilledDst = tempIntersection(dst, spills);

		while(spilledSrc)
		{
			Temp_temp temp = spilledSrc->head;
			F_access access = TAB_look(tempPos, temp);
			Temp_temp newTemp = Temp_newtemp();
			char load[INSTLENGTH];
			sprintf(load, "movq %d(`s0), `d0", F_frameLength(f) * WORDSIZE + F_accessOffset(access));
			AS_instr loadInstr = AS_Move(String(load), Temp_TempList(newTemp, NULL), Temp_TempList(F_RSP(), NULL));

		}
		while(spilledDst)
		{
			Temp_temp temp = spilledDst->head;
			F_access access = TAB_look(tempPos, temp);
			Temp_temp newTemp = Temp_newtemp();
			char store[INSTLENGTH];
			sprintf(store, "movq `s0, %d(`s1)", F_frameLength(f) * WORDSIZE + F_accessOffset(access));
			AS_instr storeInstr = AS_Move(String(store), NULL, Temp_TempList(newTemp, Temp_TempList(F_RSP(), NULL)));
		}
	}
	return result;
}

struct RA_result RA_regAlloc(F_frame f, AS_instrList il) {
	//your code here
	struct RA_result ret;
	printf("enter FG_AssemFlowGraph\n");
	G_graph fg = FG_AssemFlowGraph(il, f);
	printf("end FG_AssemFlowGraph\n");
	printf("begin Live_liveness\n");
	struct Live_graph liveness = Live_liveness(fg);
	printf("end Live_liveness\n");
	printf("start color reg\n");
	struct COL_result colorResult = COL_color(liveness.graph, F_tempMap, NULL, liveness.moves);
	printf("end color reg\n");
	if(colorResult.spills != NULL)
	{
		printf("enter rewrite\n");
		AS_instrList newIl = RewriteProgram(f, il, colorResult.spills);
		return RA_regAlloc(f, newIl);
	}

	Temp_map allRegsMap = Temp_layerMap(F_tempMap, colorResult.coloring);
	AS_instrList *instPointer = &il;
	while(*instPointer)
	{
		printf("111111111\n");
		AS_instr inst = (*instPointer)->head;
		if(inst->kind == I_MOVE && strstr(inst->u.MOVE.assem, "movq `s0, `d0"))
		{
			printf("22222222222dfsdfsf\n");
			Temp_temp src = inst->u.MOVE.src->head;
			Temp_temp dst = inst->u.MOVE.dst->head;
			printf("22222222222dfsdfssdfasdfdf\n");
			printf("src reg:%s, dst reg:%s\n", (char *)Temp_look(allRegsMap, src), (char *)Temp_look(allRegsMap, dst));
			printf("src reg:%s, dst reg:%s\n", (char *)Temp_look(Temp_name(), src), (char *)Temp_look(Temp_name(), dst));
			strcmp((char *)Temp_look(allRegsMap, src), (char *)Temp_look(allRegsMap, dst));
			printf("sdfjkalskjdf\n");
			if(strcmp((char *)Temp_look(allRegsMap, src), (char *)Temp_look(allRegsMap, dst)) == 0)
			{
				printf("sdfssssssssssssssssddddddddddddddddddddd\n");
				*instPointer = (*instPointer)->tail;
				printf("22222222222\n");
				continue;
			}
		}
		printf("asdfljkkkkkkkkkkkkkkkkkkkkkkkk\n");
		instPointer = &((*instPointer)->tail);
		printf("333333333333\n");
	}

	ret.coloring = colorResult.coloring;
	ret.il = il;
	printf("arrive hereeeeee\n");
	return ret;
}
