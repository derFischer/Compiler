#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "table.h"
#include "symbol.h"
#include "absyn.h"
#include "temp.h"
#include "tree.h"
#include "printtree.h"
#include "frame.h"
#include "translate.h"
#include "errormsg.h"

#define WORDSIZE 8
//LAB5: you can modify anything you want.
F_fragList frags = NULL;
struct Tr_access_
{
	Tr_level level;
	F_access access;
};

struct Tr_level_
{
	F_frame frame;
	Tr_level parent;
};

typedef struct patchList_ *patchList;
struct patchList_
{
	Temp_label *head;
	patchList tail;
};

struct Cx
{
	patchList trues;
	patchList falses;
	T_stm stm;
};

struct Tr_exp_
{
	enum
	{
		Tr_ex,
		Tr_nx,
		Tr_cx
	} kind;
	union {
		T_exp ex;
		T_stm nx;
		struct Cx cx;
	} u;
};

static patchList PatchList(Temp_label *head, patchList tail)
{
	patchList list;

	list = (patchList)checked_malloc(sizeof(struct patchList_));
	list->head = head;
	list->tail = tail;
	return list;
}

void doPatch(patchList tList, Temp_label label)
{
	for (; tList; tList = tList->tail)
		*(tList->head) = label;
}

patchList joinPatch(patchList first, patchList second)
{
	if (!first)
		return second;
	for (; first->tail; first = first->tail)
		;
	first->tail = second;
	return first;
}

static Tr_exp Tr_Ex(T_exp ex)
{
	Tr_exp e = malloc(sizeof(*e));
	e->kind = Tr_ex;
	e->u.ex = ex;
	return e;
}

static Tr_exp Tr_Nx(T_stm nx)
{
	Tr_exp e = malloc(sizeof(*e));
	e->kind = Tr_nx;
	e->u.nx = nx;
	return e;
}

static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm)
{
	Tr_exp e = malloc(sizeof(*e));
	e->kind = Tr_cx;
	e->u.cx.trues = trues;
	e->u.cx.falses = falses;
	e->u.cx.stm = stm;
	return e;
}

static T_exp unEx(Tr_exp e)
{
	EM_error(0, "ex kind %d\n", e->kind);
	switch (e->kind)
	{
	case Tr_ex:
		EM_error(0, "ex reach here\n");
		T_exp tmp = e->u.ex;
				EM_error(0, "ex reach here111\n");
		return tmp;
	case Tr_cx:
	{
		Temp_temp r = Temp_newtemp();
		Temp_label t = Temp_newlabel(), f = Temp_newlabel();
		doPatch(e->u.cx.trues, t);
		doPatch(e->u.cx.falses, f);
		return T_Eseq(T_Move(T_Temp(r), T_Const(1)),
					  T_Eseq(e->u.cx.stm,
							 T_Eseq(T_Label(f),
									T_Eseq(T_Move(T_Temp(r), T_Const(0)),
										   T_Eseq(T_Label(t),
												  T_Temp(r))))));
	}
	case Tr_nx:
		return T_Eseq(e->u.nx, T_Const(0));
	}
}

static T_stm unNx(Tr_exp e)
{
	switch (e->kind)
	{
	case Tr_ex:
		return T_Exp(e->u.ex);
	case Tr_nx:
		return e->u.nx;
	case Tr_cx:
		return T_Exp(unEx(e));
	}
}

static struct Cx unCx(Tr_exp e)
{
	switch (e->kind)
	{
	case Tr_ex:
	{
		struct Cx cx;
		T_stm stm = T_Cjump(T_ne, e->u.ex, T_Const(0), NULL, NULL);
		cx.trues = PatchList(&(stm->u.CJUMP.true), NULL);
		cx.falses = PatchList(&(stm->u.CJUMP.false), NULL);
		return cx;
	}
	case Tr_nx:
	{
		struct Cx tmp;
		tmp.trues = NULL;
		tmp.falses = NULL;
		tmp.stm = NULL;
		return tmp;
	}
	case Tr_cx:
		return e->u.cx;
	}
}

//constructor
Tr_access Tr_Access(Tr_level level, F_access access)
{
	Tr_access acc = malloc(sizeof(*acc));
	acc->level = level;
	acc->access = access;
	return acc;
}

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail)
{
	Tr_accessList list = malloc(sizeof(*list));
	list->head = head;
	list->tail = tail;
	return list;
}

Tr_expList Tr_ExpList(Tr_exp head, Tr_expList tail)
{
	Tr_expList list = malloc(sizeof(*list));
	list->head = head;
	list->tail = tail;
	return list;
}

Tr_level Tr_outermost(void)
{
	Tr_level level = malloc(sizeof(*level));
	level->frame = NULL;
	level->parent = NULL;
	return level;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals)
{
	Tr_level level = malloc(sizeof(*level));

	//add static link to formals
	level->frame = F_newFrame(name, U_BoolList(TRUE, formals));
	level->parent = parent;
	return level;
}

Tr_accessList Tr_formals(Tr_level level)
{
	F_accessList list = F_formals(level->frame);
	Tr_accessList tlist = malloc(sizeof(*tlist));
	Tr_accessList result = tlist;
	while (list->head)
	{
		Tr_access tmp = Tr_Access(level, list->head);
		tlist->tail = Tr_AccessList(tmp, NULL);
		list = list->tail;
		tlist = tlist->tail;
	}
	return tlist->tail;
}

Tr_access Tr_allocLocal(Tr_level level, bool escape)
{
	Tr_access access = malloc(sizeof(*access));
	access->level = level;
	access->access = F_allocLocal(level->frame, escape);
	return access;
}

Tr_exp Tr_simpleVar(Tr_access access, Tr_level level)
{
	T_exp fp = T_Temp(F_FP());
	while (level != access->level)
	{
		fp = T_Mem(T_Binop(T_plus, T_Const(2 * WORDSIZE), fp));
		level = level->parent;
	}
	return Tr_Ex(F_exp(access->access, fp));
}

Tr_exp Tr_fieldVar(Tr_exp address, int offset)
{
	return Tr_Ex(T_Mem(T_Binop(T_plus, T_Const(offset * WORDSIZE), unEx(address))));
}

Tr_exp Tr_subscriptVar(Tr_exp address, int offset)
{
	return Tr_fieldVar(address, offset);
}

Tr_exp Tr_nilExp()
{
	return Tr_Ex(T_Const(0));
}

Tr_exp Tr_intExp(int intt)
{
	return Tr_Ex(T_Const(intt));
}

Tr_exp Tr_stringExp(string stringg)
{
	Temp_label label = Temp_newlabel();
	frags = F_FragList(F_StringFrag(label, stringg), frags);
	return Tr_Ex(T_Name(label));
}

Tr_exp Tr_callExp(Temp_label fname, Tr_expList params, Tr_level caller, Tr_level callee)
{
	T_expList args = NULL;
	for (; params; params = params->tail)
	{
		args = T_ExpList(unEx(params->head), args);
	}

	T_exp fp = T_Temp(F_FP());
	while (callee != caller)
	{
		fp = T_Mem(T_Binop(T_plus, T_Const(WORDSIZE), fp));
		caller = caller->parent;
	}
	fp = T_Mem(T_Binop(T_plus, T_Const(WORDSIZE), fp));
	args = T_ExpList(fp, args);
	return Tr_Ex(T_Call(T_Name(fname), args));
}

Tr_exp Tr_opExp(A_oper oper, Tr_exp left, Tr_exp right)
{
	T_binOp op;
	switch (oper)
	{
	case A_plusOp:
		op = T_plus;
		break;
	case A_minusOp:
		op = T_minus;
		break;
	case A_timesOp:
		op = T_mul;
		break;
	case A_divideOp:
		op = T_div;
		break;
	}
	TM_error(0, "op exp here\n");
	T_exp rightr = unEx(right);
	TM_error(0, "op exp jere\n");
	return Tr_Ex(T_Binop(op, unEx(left), t));
}

Tr_exp Tr_intCompExp(A_oper op, Tr_exp left, Tr_exp right)
{
	T_relOp rop;
	switch (op)
	{
	case A_eqOp:
		rop = T_eq;
		break;
	case A_neqOp:
		rop = T_ne;
		break;
	case A_ltOp:
		rop = T_lt;
		break;
	case A_leOp:
		rop = T_le;
		break;
	case A_gtOp:
		rop = T_gt;
		break;
	case A_geOp:
		rop = T_ge;
		break;
	}
	T_stm stm = T_Cjump(rop, unEx(left), unEx(right), NULL, NULL);
	patchList trues = PatchList(&(stm->u.CJUMP.true), NULL);
	patchList falses = PatchList(&(stm->u.CJUMP.false), NULL);
	return Tr_Cx(trues, falses, stm);
}

Tr_exp stringCompExp(A_oper op, Tr_exp left, Tr_exp right)
{
	Tr_exp l = Tr_Ex(F_externalCall("stringEqual", T_ExpList(unEx(left), T_ExpList(unEx(right), NULL))));
	Tr_exp r = Tr_Ex(T_Const(0));
	return Tr_intCompExp(op, l, r);
}

Tr_exp Tr_recordExp(int size, Tr_expList list)
{
	Temp_temp addr = Temp_newtemp();
	T_stm mallocSpace = T_Move(T_Temp(addr), F_externalCall("malloc", T_ExpList(T_Const(size * WORDSIZE), NULL)));
	T_exp exp = T_Temp(addr);
	int i = 0;
	for (; list; list = list -> tail)
	{
		exp = T_Eseq(T_Move(T_Mem(T_Binop(T_plus, T_Temp(addr), T_Const(i * WORDSIZE))), unEx(list->head)), exp);
		++i;
	}
	exp = T_Eseq(mallocSpace, exp);
	return Tr_Ex(exp);
}

Tr_exp Tr_assignExp(Tr_exp lvalue, Tr_exp value)
{
	return Tr_Nx(T_Move(unEx(lvalue), unEx(value)));
}

Tr_exp Tr_ifExp(Tr_exp test, Tr_exp then, Tr_exp elsee)
{
	Temp_label t = Temp_newlabel();
	Temp_label f = Temp_newlabel();
	Temp_label finish = Temp_newlabel();
	struct Cx cx = unCx(test);
	doPatch(cx.trues, t);
	doPatch(cx.falses, f);
	Temp_temp r = Temp_newtemp();
	T_stm thenStm = T_Seq(T_Label(t), T_Seq(T_Move(T_Temp(r), unEx(then)),
											T_Jump(T_Name(finish), Temp_LabelList(finish, NULL))));
	T_stm elseStm = T_Seq(T_Label(f), T_Seq(T_Move(T_Temp(r), unEx(elsee)),
											T_Jump(T_Name(finish), Temp_LabelList(finish, NULL))));
	T_exp exp = T_Eseq(cx.stm, T_Eseq(thenStm, T_Eseq(elseStm, T_Eseq(T_Label(finish), T_Temp(r)))));
	return Tr_Ex(exp);
}

Tr_exp Tr_whileExp(Tr_exp test, Tr_exp body, Temp_label finish)
{
	Temp_label testt = Temp_newlabel();
	Temp_label bodyy = Temp_newlabel();
	struct Cx cx = unCx(test);
	doPatch(cx.trues, bodyy);
	doPatch(cx.falses, finish);

	T_stm stm = T_Seq(T_Label(testt),
					T_Seq(cx.stm, 
						T_Seq(T_Label(bodyy), 
							T_Seq(unNx(body),
								T_Seq(T_Jump(T_Name(testt), Temp_LabelList(testt, NULL)),
									T_Label(finish)))))); 
	return Tr_Nx(stm);
}

Tr_exp Tr_forExp(Tr_access access, Tr_exp lo, Tr_exp hi, Tr_exp body, Temp_label finish)
{
	Temp_temp high = Temp_newtemp();
	T_exp highest = T_Temp(high);
	T_exp loop = F_exp(access->access, T_Temp(F_FP()));
	T_stm initStm = T_Seq(T_Move(highest, unEx(hi)), T_Move(loop, unEx(lo)));

	Temp_label loopBody = Temp_newlabel();
	Temp_label looptest = Temp_newlabel();
	T_stm loopStm = T_Cjump(T_le, loop, highest, loopBody, finish);
	T_stm bodyStm = T_Seq(unNx(body), T_Seq(T_Move(loop, T_Binop(T_plus, loop, T_Const(1))), 
											T_Jump(T_Name(looptest), Temp_LabelList(looptest, NULL))));
	T_stm forStm = T_Seq(T_Label(looptest),
							T_Seq(loopStm,
								T_Seq(T_Label(loopBody),
									T_Seq(bodyStm,
										T_Label(finish)))));
	return Tr_Nx(T_Seq(initStm, forStm));
}

Tr_exp Tr_breakExp(Temp_label label)
{
	return Tr_Nx(T_Jump(T_Name(label), Temp_LabelList(label, NULL)));
}
Tr_exp Tr_arrayExp(Tr_exp size, Tr_exp init)
{
	Temp_temp r = Temp_newtemp();
	return Tr_Ex(T_Eseq(T_Move(T_Temp(r), F_externalCall("initArray", T_ExpList(T_Binop(T_mul, unEx(size), T_Const(WORDSIZE)), T_ExpList(unEx(init), NULL)))), T_Temp(r)));
}

void Tr_procEntryExit1(Tr_level level, Tr_exp body, Tr_accessList formals)
{
	T_stm exitStm = T_Move(T_Temp(F_RV()), unEx(body));
	F_frag frag = F_ProcFrag(exitStm, level->frame);
	frags = F_FragList(frag, frags);
	return;
}

F_fragList Tr_getResult(void)
{
	return frags;
}