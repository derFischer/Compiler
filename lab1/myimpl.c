#include "prog1.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

int calculateExpListLength(A_expList expList);
int maxArgsInExpList(A_expList expList);
int maxArgsInExp(A_exp exp);
int maxargs(A_stm stm);

int compare(int a, int b)
{
	if (a > b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

//calculate the number of args of a print stm
int calculateExpListLength(A_expList expList)
{
	int i = 0;
	while(1)
	{
		if (expList->kind == A_lastExpList)
		{
			++i;
			return i;
		}
		else
		{
			++i;
			expList = expList->u.pair.tail;
		}
	}
}

//calculate the max args in a explist
int maxArgsInExpList(A_expList expList)
{
	if (expList->kind == A_lastExpList)
	{
		return maxArgsInExp(expList->u.last);
	}
	else
	{
		return compare(maxArgsInExp(expList->u.pair.head), maxArgsInExpList(expList->u.pair.tail));
	}
}

//calculate the max args in a exp
int maxArgsInExp(A_exp exp)
{
	switch (exp->kind)
	{
		case A_idExp:
		case A_numExp:
			return 0;
		case A_opExp:
			return compare(maxArgsInExp(exp->u.op.left), maxArgsInExp(exp->u.op.right));
		case A_eseqExp:
			return compare(maxArgsInExp(exp->u.eseq.exp), maxargs(exp->u.eseq.stm));
	}
}

//calculate the max args in a stm
int maxargs(A_stm stm)
{
	switch (stm->kind)
	{
		case A_assignStm:
			return maxArgsInExp(stm->u.assign.exp);
		case A_printStm:
			return compare(calculateExpListLength(stm->u.print.exps), maxArgsInExpList(stm->u.print.exps));
		case A_compoundStm:
			return compare(maxargs(stm->u.compound.stm1), maxargs(stm->u.compound.stm2));
	}
}

//a list, to store the value of identifiers
typedef struct table *Table_;

struct table 
{
	string id;
	int value;
	Table_ tail;
};

Table_ Table(string id, int value, struct table *tail)
{
	Table_ t = malloc(sizeof(*t));
	t->id = id;
	t->value = value;
	t->tail = tail;
	return t;
}

typedef struct IntAndTable intandtable; 
struct IntAndTable
{
	int i;
	Table_ t;
};

int lookup(string id, Table_ t);
Table_ printStm (A_expList expList, Table_ t);
intandtable interpExp(A_exp exp, Table_ t);
Table_ interpStm(A_stm s, Table_ t);


int lookup(string id, Table_ t)
{
	while (t != NULL)
	{
		if (strcmp(t->id, id) == 0)
		{
			return t->value;
		}
		t = t->tail;
	}
}

Table_ printStm (A_expList expList, Table_ t)
{
	Table_ tmp = t;
	while(1)
	{
		if (expList->kind == A_lastExpList)
		{
			intandtable result = interpExp(expList->u.last, tmp);
			printf("%d\n", result.i);
			return result.t;
		}
		else
		{
			intandtable result = interpExp(expList->u.pair.head, tmp);
			printf("%d ", result.i);
			tmp = result.t;
			expList = expList->u.pair.tail;
		}
	}
}

intandtable interpExp(A_exp exp, Table_ t)
{
	Table_ tmp = t;
	intandtable result;
	intandtable tmp1;
	intandtable tmp2;
	switch (exp->kind)
	{
		case A_idExp:
			result.i = lookup(exp->u.id, tmp);
			result.t = tmp;
			return result;
		case A_numExp:
			result.i = exp->u.num;
			result.t = tmp;
			return result;
		case A_opExp:
			tmp1 = interpExp(exp->u.op.left, tmp);
			tmp = tmp1.t;
			tmp2 = interpExp(exp->u.op.right, tmp);
			tmp = tmp2.t;
			result.t = tmp;
			switch (exp->u.op.oper)
			{
				case A_plus:
					result.i = tmp1.i + tmp2.i;
					return result;
				case A_minus:
					result.i = tmp1.i - tmp2.i;
					return result;
				case A_times:
					result.i = tmp1.i * tmp2.i;
					return result;
				case A_div:
					result.i = tmp1.i / tmp2.i;
					return result;
			}
		case A_eseqExp:
			tmp = interpStm(exp->u.eseq.stm, tmp);
			return interpExp(exp->u.eseq.exp, tmp);
	}
}

Table_ interpStm(A_stm stm, Table_ t)
{
	Table_ tmp = t;
	intandtable result;
	switch (stm->kind)
	{
		case A_compoundStm:
			tmp = interpStm(stm->u.compound.stm1, tmp);
			tmp = interpStm(stm->u.compound.stm2, tmp);
			break;
		case A_assignStm:
			result = interpExp(stm->u.assign.exp, tmp);
			tmp = result.t;
			tmp = Table(stm->u.assign.id, result.i, tmp);
			break;
		case A_printStm:
			tmp = printStm(stm->u.print.exps, tmp);
			break;
	}
	return tmp;
}

void interp(A_stm stm)
{
	//TODO: put your code here.
	interpStm(stm, NULL);
}
