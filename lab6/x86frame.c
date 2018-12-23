#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "table.h"
#include "tree.h"
#include "frame.h"
#include "liveness.h"

/*Lab5: Your implementation here.*/
#define WORDSIZE 8

Temp_temp rsp = NULL;
Temp_temp rbp = NULL;
Temp_temp rax = NULL;
Temp_temp rdi = NULL;
Temp_temp rsi = NULL;
Temp_temp rdx = NULL;
Temp_temp rcx = NULL;
Temp_temp r8 = NULL;
Temp_temp r9 = NULL;
Temp_temp rbx = NULL;
Temp_temp r12 = NULL;
Temp_temp r13 = NULL;
Temp_temp r14 = NULL;
Temp_temp r15 = NULL;
Temp_temp r10 = NULL;
Temp_temp r11 = NULL;

Temp_temp F_RSP(void);
Temp_temp F_RBP(void);
Temp_temp F_RAX(void);
Temp_temp F_RDI(void);
Temp_temp F_RSI(void);
Temp_temp F_RDX(void);
Temp_temp F_RCX(void);
Temp_temp F_R8(void);
Temp_temp F_R9(void);
Temp_temp F_R10(void);
Temp_temp F_R11(void);
Temp_temp F_R12(void);
Temp_temp F_R13(void);
Temp_temp F_R14(void);
Temp_temp F_R15(void);
Temp_temp F_RBX(void);

//varibales
struct F_frame_
{
	Temp_label name;

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
};

struct F_access_
{
	enum
	{
		inFrame,
		inReg
	} kind;
	union {
		int offset;	//inFrame
		Temp_temp reg; //inReg
	} u;
};
static F_access InFrame(int offset)
{
	F_access access = malloc(sizeof(*access));
	access->kind = inFrame;
	access->u.offset = offset;
	return access;
}

static F_access inRegg(Temp_temp reg)
{
	F_access access = malloc(sizeof(*access));
	access->kind = inReg;
	access->u.reg = reg;
	return access;
}

F_accessList F_AccessList(F_access head, F_accessList tail)
{
	F_accessList list = malloc(sizeof(*list));
	list->head = head;
	list->tail = tail;
	return list;
}

F_fragList F_FragList(F_frag head, F_fragList tail)
{
	F_fragList list = malloc(sizeof(*list));
	list->head = head;
	list->tail = tail;
	return list;
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
	int frame_offset = WORDSIZE;

	F_accessList list = malloc(sizeof(*list));
	F_accessList tmp = list;
	while (formals)
	{
		if (formals->head)
		{
			F_access access = inRegg(Temp_newtemp());
			tmp->tail = F_AccessList(access, NULL);
		}
		else
		{
			F_access access = InFrame(frame_offset);
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
	if (escape)
	{
		access = InFrame(-WORDSIZE * (frame->length));
	}
	else
	{
		access = inRegg(Temp_newtemp());
	}
	frame->locals = F_AccessList(access, frame->locals);
	++frame->length;
	return access;
}

T_exp F_exp(F_access acc, T_exp framePtr)
{
	switch (acc->kind)
	{
	case inFrame:
		return T_Mem(T_Binop(T_plus, T_Const(acc->u.offset), framePtr));
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
	return stm;
}

F_frag F_StringFrag(Temp_label label, string str)
{
	F_frag f = malloc(sizeof(*f));
	f->kind = F_stringFrag;
	f->u.stringg.label = label;
	f->u.stringg.str = str;

	return f;
}

F_frag F_ProcFrag(T_stm body, F_frame frame)
{
	F_frag f = malloc(sizeof(*f));
	f->kind = F_procFrag;
	f->u.proc.body = body;
	f->u.proc.frame = frame;

	return f;
}

Temp_temp F_FP(void)
{
	return F_RBP();
}

Temp_temp F_RV(void)
{
	return F_RAX();
}

Temp_temp F_argsReg(int index)
{
	switch (index)
	{
	case 1:
		return F_RDI();
	case 2:
		return F_RSI();
	case 3:
		return F_RDX();
	case 4:
		return F_RCX();
	case 5:
		return F_R8();
	case 6:
		return F_R9();
	default:
		printf("wrong index of a arg reg\n");
		assert(0);
	}
}

Temp_tempList F_callersaves()
{
	return Temp_TempList(F_R10(),
						 Temp_TempList(F_R11(),
									   Temp_TempList(F_RDI(),
													 Temp_TempList(F_RSI(),
																   Temp_TempList(F_RDX(),
																				 Temp_TempList(F_RCX(),
																							   Temp_TempList(F_R8(),
																											 Temp_TempList(F_R9(), NULL))))))));
}

Temp_tempList F_calleesaves()
{
	return Temp_TempList(F_RBX(),
						 Temp_TempList(F_RBP(),
									   Temp_TempList(F_R12(),
													 Temp_TempList(F_R13(),
																   Temp_TempList(F_R14(),
																				 Temp_TempList(F_R15(), NULL))))));
}

Temp_tempList F_allRegisters()
{
	Temp_tempList calleesaves = F_calleesaves();
	Temp_tempList callersaves = F_callersaves();
	Temp_tempList allRegisters = L_tempListUnion(calleesaves, callersaves);
	free(calleesaves);
	free(callersaves);
	Temp_tempList specialregs = Temp_TempList(F_RSP(), Temp_TempList(F_RBP(), NULL));
	return L_tempListMinus(allRegisters, specialregs);
}

//registers
Temp_temp F_RSP(void)
{
	if (rsp == NULL)
	{
		rsp = Temp_newtemp();
		Temp_enter(F_tempMap, rsp, "%rsp");
	}
	return rsp;
}

Temp_temp F_RBP(void)
{
	if (rbp == NULL)
	{
		rbp = Temp_newtemp();
		Temp_enter(F_tempMap, rbp, "%rbp");
	}
	return rbp;
}

Temp_temp F_RAX(void)
{
	if (rax == NULL)
	{
		rax = Temp_newtemp();
		Temp_enter(F_tempMap, rax, "%rax");
	}
	return rax;
}

Temp_temp F_RDI(void)
{
	if (rdi == NULL)
	{
		rdi = Temp_newtemp();
		Temp_enter(F_tempMap, rdi, "%rdi");
	}
	return rdi;
}

Temp_temp F_RSI(void)
{
	if (rsi == NULL)
	{
		rsi = Temp_newtemp();
		Temp_enter(F_tempMap, rsi, "%rsi");
	}
	return rsi;
}

Temp_temp F_RDX(void)
{
	if (rdx == NULL)
	{
		rdx = Temp_newtemp();
		Temp_enter(F_tempMap, rdx, "%rdx");
	}
	return rdx;
}

Temp_temp F_RCX(void)
{
	if (rcx == NULL)
	{
		rcx = Temp_newtemp();
		Temp_enter(F_tempMap, rcx, "%rcx");
	}
	return rcx;
}

Temp_temp F_R8(void)
{
	if (r8 == NULL)
	{
		r8 = Temp_newtemp();
		Temp_enter(F_tempMap, r8, "%r8");
	}
	return r8;
}

Temp_temp F_R9(void)
{
	if (r9 == NULL)
	{
		r9 = Temp_newtemp();
		Temp_enter(F_tempMap, r9, "%r9");
	}
	return r9;
}

Temp_temp F_RBX(void)
{
	if (rbx == NULL)
	{
		rbx = Temp_newtemp();
		Temp_enter(F_tempMap, rbx, "%rbx");
	}
	return rbx;
}

Temp_temp F_R12(void)
{
	if (r12 == NULL)
	{
		r12 = Temp_newtemp();
		Temp_enter(F_tempMap, r12, "%r12");
	}
	return r12;
}

Temp_temp F_R13(void)
{
	if (r13 == NULL)
	{
		r13 = Temp_newtemp();
		Temp_enter(F_tempMap, r13, "%r13");
	}
	return r13;
}

Temp_temp F_R14(void)
{
	if (r14 == NULL)
	{
		r14 = Temp_newtemp();
		Temp_enter(F_tempMap, r14, "%r14");
	}
	return r14;
}

Temp_temp F_R15(void)
{
	if (r15 == NULL)
	{
		r15 = Temp_newtemp();
		Temp_enter(F_tempMap, r15, "%r15");
	}
	return r15;
}

Temp_temp F_R10(void)
{
	if (r10 == NULL)
	{
		r10 = Temp_newtemp();
		Temp_enter(F_tempMap, r10, "%r10");
	}
	return r10;
}

Temp_temp F_R11(void)
{
	if (r11 == NULL)
	{
		r11 = Temp_newtemp();
		Temp_enter(F_tempMap, r11, "%r11");
	}
	return r11;
}

int F_accessOffset(F_access access)
{
	if(access->kind == inFrame)
	{
		return access->u.offset;
	}
	else
	{
		printf("temp access doesnt have an offset\n");
		assert(0);
	}
}

int F_frameLength(F_frame f)
{
	return f->length;
}