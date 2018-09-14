#include "prog1.h"
#include "string.h"
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
		if (expList->kind == A_lastExp)
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
		return maxArgsInExp(expList->u.last)
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
			return compare(maxArgsInExp(exp->u.op.left), maxArgsInExp(exp->u.left));
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
		case A_CompoundStm:
			return compare(maxargs(stm->u.compound.stm1), maxargs(stm->u.compound.stm2));
	}
}

//a list, to store the value of identifiers
typedef struct
{
	string id
	int value;
	node *next;
} node

int getValueOfIdentifier(string id)
{

}

void modifiyValueOfIdentifier(string id, int value)
{

}

void printStm (A_expList expList)
{
	while(1)
	{
		if (expList->kind == A_lastExp)
		{
			printf("%d\n", interExp())
			return
		}
		else
		{
			expList = expList->u.pair.tail;
		}
	}
}

int interpExp(A_exp exp)
{
	switch (exp->kind)
	{
		case A_idExp:
			return getValueOfIdentifier(exp->u.id);
		case A_numExp:
			return exp->u.num;
		case A_opExp:
			switch (exp->u.op.oper)
			{
				case A_plus:
					return interpExp(exp->u.op.left) + interpExp(exp->u.op.right);
				case A_minus:
					return interpExp(exp->u.op.left) - interpExp(exp->u.op.right);
				case A_times:
					return interpExp(exp->u.op.left) * interpExp(exp->u.op.right);
				case A_div:
					return interpExp(exp->u.op.left) / interpExp(exp->u.op.right);
			}
		case A_eseqExp:
			interp(exp->u.eseq.stm);
			return interpExp(exp->u.eseq.exp);
	}
}

void interp(A_stm stm)
{
	//TODO: put your code here.

}
