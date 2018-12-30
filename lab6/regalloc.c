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
#include "helper.h"
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

Temp_tempList RewriteInstr(Temp_tempList old, Temp_temp spillTemp, Temp_temp newTemp)
{
	Temp_tempList result = old;
	for(; old; old = old->tail)
	{
		if(old->head == spillTemp)
		{
			old->head = newTemp;
		}
	}
	return result;
}

void modifySrc(AS_instr instr, Temp_temp spill, Temp_temp new)
{
	switch(instr->kind)
	{
		case I_OPER:
		instr->u.OPER.src = RewriteInstr(instr->u.OPER.src, spill, new);
		break;
		case I_MOVE:
		instr->u.MOVE.src = RewriteInstr(instr->u.MOVE.src, spill, new);
		break;
	}
}

void modifyDst(AS_instr instr, Temp_temp spill, Temp_temp new)
{
	switch(instr->kind)
	{
		case I_OPER:
		instr->u.OPER.dst = RewriteInstr(instr->u.OPER.dst, spill, new);
		break;
		case I_MOVE:
		instr->u.MOVE.dst = RewriteInstr(instr->u.MOVE.dst, spill, new);
		break;
	}
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
			dst = inst->u.MOVE.dst;
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
			AS_instr loadInstr = AS_Oper(String(load), Temp_TempList(newTemp, NULL), Temp_TempList(F_RSP(), NULL), NULL);
			modifySrc(inst, temp, newTemp);
			il->head = loadInstr;
			il->tail = AS_InstrList(inst,il->tail);
			il = il->tail;
			spilledSrc = spilledSrc->tail;
		}
		while(spilledDst)
		{
			Temp_temp temp = spilledDst->head;
			F_access access = TAB_look(tempPos, temp);
			Temp_temp newTemp = Temp_newtemp();
			char store[INSTLENGTH];
			sprintf(store, "movq `s0, %d(`s1)", F_frameLength(f) * WORDSIZE + F_accessOffset(access));
			AS_instr storeInstr = AS_Oper(String(store), NULL, Temp_TempList(newTemp, Temp_TempList(F_RSP(), NULL)), NULL);
			modifyDst(inst, temp, newTemp);
			il->tail = AS_InstrList(storeInstr,il->tail);
			spilledDst = spilledDst->tail;
		}
		il = il->tail;
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
		showTempList(colorResult.spills);
		printf("----------------------old------------------\n");
		AS_printInstrList(stdout,il,Temp_layerMap(F_tempMap,Temp_name()));
		AS_instrList newIl = RewriteProgram(f, il, colorResult.spills);
		printf("----------------------new------------------\n");
		AS_printInstrList(stdout,newIl,Temp_layerMap(F_tempMap,Temp_name()));
		return RA_regAlloc(f, newIl);
	}

	Temp_map allRegsMap = Temp_layerMap(F_tempMap, colorResult.coloring);
	AS_instrList *instPointer = &il;
	while(*instPointer)
	{
		AS_instr inst = (*instPointer)->head;
		if(inst->kind == I_MOVE && strstr(inst->u.MOVE.assem, "movq `s0, `d0"))
		{
			Temp_temp src = inst->u.MOVE.src->head;
			Temp_temp dst = inst->u.MOVE.dst->head;
			//printf("src reg:%s, dst reg:%s\n", (char *)Temp_look(allRegsMap, src), (char *)Temp_look(allRegsMap, dst));
			//printf("src reg:%s, dst reg:%s\n", (char *)Temp_look(Temp_name(), src), (char *)Temp_look(Temp_name(), dst));
			strcmp((char *)Temp_look(allRegsMap, src), (char *)Temp_look(allRegsMap, dst));
			if(strcmp((char *)Temp_look(allRegsMap, src), (char *)Temp_look(allRegsMap, dst)) == 0)
			{
				*instPointer = (*instPointer)->tail;
				continue;
			}
		}
		instPointer = &((*instPointer)->tail);
	}

	ret.coloring = colorResult.coloring;
	ret.il = il;
	return ret;
}
