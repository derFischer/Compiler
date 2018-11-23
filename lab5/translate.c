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

static Tr_exp Tr_Ex(T_exp ex);
static Tr_exp Tr_Nx(T_stm nx);
static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm);

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