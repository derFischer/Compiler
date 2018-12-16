#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "util.h"
#include "absyn.h"
#include "temp.h"
#include "frame.h"

/* Lab5: your code below */
typedef struct Tr_exp_ *Tr_exp;

typedef struct Tr_access_ *Tr_access;

typedef struct Tr_accessList_ *Tr_accessList;

typedef struct Tr_level_ *Tr_level;

typedef struct Tr_expList_ *Tr_expList;

struct Tr_accessList_
{
	Tr_access head;
	Tr_accessList tail;
};

struct Tr_expList_
{
	Tr_exp head;
	Tr_expList tail;
};

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail);

Tr_level Tr_outermost(void);

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);

Tr_accessList Tr_formals(Tr_level level);

Tr_access Tr_allocLocal(Tr_level level, bool escape);

/*
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
*/
Tr_expList Tr_ExpList(Tr_exp head, Tr_expList tail);

Tr_exp Tr_simpleVar(Tr_access access, Tr_level level);
Tr_exp Tr_fieldVar(Tr_exp address, int offset);
Tr_exp Tr_subscriptVar(Tr_exp address, int offset);

Tr_exp Tr_nilExp();
Tr_exp Tr_intExp(int intt);
Tr_exp Tr_stringExp(string stringg);
Tr_exp Tr_callExp(Temp_label fname, Tr_expList params, Tr_level caller, Tr_level callee);
Tr_exp Tr_opExp(A_oper oper, Tr_exp left, Tr_exp right);
Tr_exp Tr_intCompExp(A_oper op, Tr_exp left, Tr_exp right);
Tr_exp Tr_stringCompExp(A_oper op, Tr_exp left, Tr_exp right);
Tr_exp Tr_recordExp(int size, Tr_expList list);
Tr_exp Tr_assignExp(Tr_exp lvalue, Tr_exp value);
Tr_exp Tr_ifExp(Tr_exp test, Tr_exp then, Tr_exp elsee);
Tr_exp Tr_whileExp(Tr_exp test, Tr_exp body, Temp_label finish);
Tr_exp Tr_forExp(Tr_access access, Tr_exp lo, Tr_exp hi, Tr_exp body, Temp_label finish);
Tr_exp Tr_breakExp(Temp_label label);
Tr_exp Tr_arrayExp(Tr_exp size, Tr_exp init);

void Tr_procEntryExit1(Tr_level level, Tr_exp body, Tr_accessList formals);
F_fragList Tr_getResult(void);
#endif
