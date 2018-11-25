#include <stdio.h>
#include "util.h"
#include "table.h"
#include "symbol.h"
#include "absyn.h"
#include "temp.h"
#include "tree.h"
#include "printtree.h"
#include "frame.h"
#include "translate.h"

//LAB5: you can modify anything you want.

struct Tr_access_ {
	Tr_level level;
	F_access access;
};


struct Tr_accessList_ {
	Tr_access head;
	Tr_accessList tail;	
};

struct Tr_level_ {
	F_frame frame;
	Tr_level parent;
};

struct Tr_expList_ {
	Tr_exp head;
	Tr_expList tail;
}

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

struct Tr_exp_ {
	enum {Tr_ex, Tr_nx, Tr_cx} kind;
	union {T_exp ex; T_stm nx; struct Cx cx; } u;
};


static patchList PatchList(Temp_label *head, patchList tail)
{
	patchList list;

	list = (patchList)checked_malloc(sizeof(struct patchList_));
	list->head = head;
	list->tail = tail;
	return list;
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
	switch(e->kind)
	{
		case Tr_ex:
		return e->u.ex;
		case Tr_cx:
		{
			Temp_temp r = Temp_newtemp();
			Temp_label t = Temp_newlabel, f = Temp_newlabel();
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
	switch(e->kind)
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
	switch(e->kind)
	{
		case Tr_ex:
		{
			struct Cx cx;
			T_stm stm = T_Cjump(T_ne, e->u.ex, T_Const(0), NULL, NULL);
			cx.trues = PatchList(&(stm->u.CJUMP.true), NULL);
			cx.falses = PatchList(&(stm->u.CJUMP.false), NULL);
		}
		case Tr_nx:
		return Tr_Cx(NULL, NULL, NULL);
		case Tr_cx:
		return e->u.cx;
	}
}

void doPatch(patchList tList, Temp_label label)
{
	for(; tList; tList = tList->tail)
		*(tList->head) = label;
}

patchList joinPatch(patchList first, patchList second)
{
	if(!first) return second;
	for(; first->tail; first = first->tail);
	first->tail = second;
	return first;
}

//constructor
Tr_access Tr_Access(Tr_level level, F_access access)
{
	Tr_access acc = malloc(sizeof(*access));
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
	Temp_label lab = Temp_newlabel();
	level->frame = F_newFrame(lab, NULL);
	level->parent = NULL;
	return level;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals)
{
	Tr_level level = malloc(sizeof(*level));

	//add static link to formals
	level->frame = F_newFrame(name, U_boolList(1, formals));
	level->parent = parent;
	return level;
}

Tr_accessList Tr_formals(Tr_level level)
{
	F_accessList list = F_formals(level->frame);
	Tr_accessList tlist = malloc(sizeof(*tlist));
	Tr_accessList result = tlist;
	while(list->head)
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
	return Tr_access;
}

Tr_exp Tr_simpleVar(Tr_access access, Tr_level level)
{
	T_exp fp = T_TEMP(F_FP());
	while(l != access->level)
	{
		fp = T_Mem(T_Binop(T_plus, T_Const(2 * WORDSIZE), fp));
		l = l->parent;
	}
	return Tr_Ex(F_exp(access->access, fp));
}

Tr_exp Tr_fieldVar(Tr_exp address, int offset)
{
	return Tr_Ex(T_Mem(T_Binop(T_plus, T_Const(offset * WORDSIZE), unEx(unEx(address))));
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
	RETURN Tr_Ex(T_Const(intt));
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
	for(; params; params = params->tail)
	{
		args = T_ExpList(params->head, args);
	}

	T_exp fp = T_Temp(F_FP());
	while(callee != caller)
	{
		fp = T_Mem(T_Binop(T_plus, T_Const(WORDSIZE), fp));
		caller = caller->parent;
	}
	fp = T_Mem(T_Binop(T_plus, T_Const(WORDSIZE), fp));
	args = T_ExpList(fp, args);
	return Tr_Ex(T_Call(T_Name(fname), args));
}

Tr_exp Tr_opExp(A_oper oper, A_exp left, A_exp right)
{
	T_binOp op;
	switch(oper)
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
	return Tr_Ex(T_BinOp(op, unEx(left), unEx(right)));
}

Tr_exp Tr_intCompExp(A_oper op, Tr_exp left, Tr_exp right)
{
	T_relOp rop;
	switch(op)
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
	T_relOp rop;
	switch(op)
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
}

Tr_exp Tr_recordExp(Tr_expList list)
{

}

Tr_exp Tr_assignExp(Tr_exp lvalue, Tr_exp value)
{
	return Tr_Nx(T_Move(unEx(lvalue), value));
}

Tr_exp Tr_ifExp(Tr_exp test, Tr_exp then, Tr_exp elsee)
{
	Temp_label t = Temp_newlabel();
	Temp_label f = Temp_newlabel();
	struct Cx cx = unCx(test);
	doPatch(cx.trues, t);
	doPatch(cx.falses, f);
}
Tr_exp Tr_whileExp(A_exp test, A_exp body);
Tr_exp Tr_forExp(S_symbol var, A_exp lo, A_exp hi, A_exp body, bool escape);
Tr_exp Tr_letExp(A_decList decs, A_exp body);
Tr_exp Tr_arrayExp(S_symbol typ, A_exp size, A_exp init);