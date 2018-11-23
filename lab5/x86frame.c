#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "table.h"
#include "tree.h"
#include "frame.h"

/*Lab5: Your implementation here.*/
#define WORDSIZE 4
//varibales
struct F_frame_ {
	Temp_lable name;

	F_accessList formals;
	F_accessList locals;

	//the number of arguments
	int argSize;

	//the number of local variables
	int length;

	//register lists for the frame
	Temp_tempList calleesaves;
	Temp_tempList callersaves;
	Temp_tempList specialregs;
}

struct F_access_ {
	enum {inFrame, inReg} kind;
	union {
		int offset; //inFrame
		Temp_temp reg; //inReg
	} u;
};
static F_access InFrame(int offset)
{
	F_access access = malloc(sizeof(*access));
	access->kind = inFrame;
	access->offset = offset;
	return access;
}

static F_access inReg(Temp_temp reg)
{
	F_access access = malloc(sizeof(*access));
	access->kind = inReg;
	access->reg = reg;
	return access;
}

F_accessList F_AccessList(F_access head, F_accessList tail)
{
	F_accessList list = malloc(sizeof(*list));
	list->head = head;
	list->tail = tail;
	return list;
}

F_frag F_StringFrag(Temp_label label, string str) {   
	    return NULL;                                      
}                                                     
                                                      
F_frag F_ProcFrag(T_stm body, F_frame frame) {        
	    return NULL;                                      
}                                                     
                                                      
F_fragList F_FragList(F_frag head, F_fragList tail) { 
	    return NULL;                                      
}                                                     

F_frame F_newFrame(Temp_label name, U_boolList formals)
{
	F_frame frame = malloc(sizeof(*frame));
	frame->name = name;
	frame->locals = NULL;
	frame->length = 0;
	frame->calleesaves = NULL;
	frame->callersaves = NULL;
	frame->specialregs = NULL;

	int argSize = 0;

	F_accessList list = malloc(sizeof(*list));
	F_accessList tmp = list;
	while(formals)
	{
		if(formals->head)
		{
			F_access access = inReg(Temp_newtemp());
			tmp->tail = F_AccessList(access, NULL);
		}
		else
		{
			F_access access = inFrame(frame_offset);
			tmp->tail = F_AccessList(access, NULL);
			frame_offset += WORDSIZE;
		}
		tmp = tmp->tail;
		formals = formals->tail;
		++argSize;
	}
	frame->formals = list->tail;
	frame->argSize = argSize;

	return frame;
}

Temp_label F_name(F_frame f)
{
	return f->name;
}

F_accessList F_formals(F_frame f)
{
	return f->formals;
}

F_access F_allocLocal(F_frame frame, bool escape)
{
	F_access access;
	if(escape)
	{
		access = inFrame(-WORDSIZE * (frame->length));
	}
	else
	{
		access = inReg(Temp_newtemp());
	}
	frame->locals = F_AccessList(access, frame->locals);
	++frame->length;
	return access;
}

Temp_temp F_FP(void)
{
	return Temp_newtemp();
}

Temp_temp F_RV(void)
{
	return Temp_newtemp();
}

T_exp F_exp(F_access acc, T_exp framePtr)
{
	switch(acc->kind)
	{
		case inFrame:
		return T_Men(T_Binop(T_plus, T_Const(acc->u.offset), framePtr));
		case inReg:
		return T_Temp(acc->u.reg);
	}
}

T_exp F_externalCall(string s, T_expList args)
{
	return T_Call(T_Name(Temp_namedlabel(s)), args);
}

T_stm F_procEntryExit1(F_frame frame, T_stm stm)
{

}

F_frag F_StringFrag(Temp_label label, string str)
{

}

F_frag F_procFrag(T_stm body, F_frame frame)
{

}