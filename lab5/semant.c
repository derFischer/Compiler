#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"
#include "types.h"
#include "helper.h"
#include "env.h"
#include "semant.h"
#include "translate.h"
#include "escape.h"

/*Lab4: Your implementation of lab4*/

struct expty
{
	Tr_exp exp;
	Ty_ty ty;
};

//In Lab4, the first argument exp should always be **NULL**.
struct expty expTy(Tr_exp exp, Ty_ty ty)
{
	struct expty e;

	e.exp = exp;
	e.ty = ty;

	return e;
}

Ty_ty actual_ty(Ty_ty t)
{
	for (; t && t->kind == Ty_name; t = t->u.name.ty)
		;
	return t;
}

struct expty transSimpleVar(S_table venv, S_table tenv, A_var v)
{
	E_enventry value = S_look(venv, v->u.simple);
	if (value && value->kind == E_varEntry)
	{
		return expTy(NULL, actual_ty(value->u.var.ty));
	}
	else
	{
		EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
		return expTy(NULL, Ty_Int());
	}
}

struct expty transVar(S_table venv, S_table tenv, A_var v, Tr_level level, Temp_label label)
{
	switch (v->kind)
	{
	case A_simpleVar:
	{
		return transSimpleVar(venv, tenv, v);
	}

	case A_fieldVar:
	{
		struct expty tmp = transVar(venv, tenv, v->u.field.var, level, label);
		if (tmp.ty->kind != Ty_record)
		{
			EM_error(v->pos, "it is not a record type");
			return expTy(NULL, Ty_Int());
		}

		Ty_fieldList t = tmp.ty->u.record;
		for (; t && t->head->name != v->u.field.sym; t = t->tail)
			;
		if (!t)
		{
			EM_error(v->pos, "field %s doesn't exist", S_name(v->u.field.sym));
			return expTy(NULL, Ty_Int());
		}
		return expTy(NULL, actual_ty(t->head->ty));
	}
	case A_subscriptVar:
	{
		struct expty tmp = transVar(venv, tenv, v->u.subscript.var, level, label);
		if (tmp.ty->kind != Ty_array)
		{
			EM_error(v->pos, "array type required");
			return expTy(NULL, Ty_Int());
		}
		struct expty t = transExp(venv, tenv, v->u.subscript.exp, level, label);
		if (t.ty->kind != Ty_int)
		{
			EM_error(v->pos, "it is not an int type");
			return expTy(NULL, Ty_Int());
		}
		return expTy(NULL, actual_ty(tmp.ty->u.array));
	}
	}
}

Tr_expList explistSame(S_table venv, S_table tenv, A_expList args, Ty_tyList formals, A_exp a, Tr_level level, Temp_label label)
{
	Tr_expList list = Tr_ExpList(NULL, NULL);
	Tr_expList tmpList = list;
	for (; args && formals; args = args->tail, formals = formals->tail)
	{
		A_exp tmp = args->head;
		Ty_ty t = formals->head;
		struct expty tt = transExp(venv, tenv, tmp, level, label);
		if (actual_ty(tt.ty) != actual_ty(t))
		{
			EM_error(tmp->pos, "para type mismatch");
			return list->tail;
		}
		tmpList->tail = Tr_ExpList(tt.exp, NULL);
		tmpList = tmpList->tail;
	}
	if (!args && !formals)
	{
		return list->tail;
	}

	if (args)
	{
		EM_error(a->pos, "too many params in function %s", S_name(a->u.call.func));
	}
	else
	{
		EM_error(a->pos, "too few params in function %s", S_name(a->u.call.func));
	}
	return list->tail;
}

Tr_expList fieldlistSame(S_table venv, S_table tenv, A_efieldList inputs, Ty_fieldList actuals, A_exp a, Tr_level level, Temp_label label)
{
	Tr_expList list = Tr_ExpList(NULL, NULL);
	Tr_expList tmpList = list;
	for (; inputs && actuals; inputs = inputs->tail, actuals = actuals->tail)
	{
		A_efield tmp = inputs->head;
		Ty_field t = actuals->head;
		if (strcmp(S_name(tmp->name), S_name(t->name)))
		{
			EM_error(a->pos, "field %s doesn't exist", S_name(tmp->name));
			return list->tail;
		}

		struct expty input = transExp(venv, tenv, tmp->exp, level, label);
		if (actual_ty(input.ty) != actual_ty(t->ty))
		{
			EM_error(a->pos, "wrong exp type");
			return list->tail;
		}
		tmpList->tail = Tr_ExpList(input.exp, NULL);
		tmpList = tmpList->tail;
	}

	if (!inputs && !actuals)
	{
		return list->tail;
	}
	else
	{
		EM_error(a->pos, "number of args wrong");
		return list->tail;
	}
}

struct expty transExp(S_table venv, S_table tenv, A_exp a, Tr_level level, Temp_label label)
{
	switch (a->kind)
	{
	case A_varExp:
	{
		return transVar(venv, tenv, a->u.var, level, label);
	}
	case A_nilExp:
	{
		return expTy(Tr_nilExp(), Ty_Nil());
	}
	case A_intExp:
	{
		return expTy(Tr_intExp(a->u.intt), Ty_Int());
	}
	case A_stringExp:
	{
		return expTy(Tr_stringExp(a->u.stringg), Ty_String());
	}
	case A_callExp:
	{
		E_enventry func = S_look(venv, a->u.call.func);
		if (!func || func->kind != E_funEntry)
		{
			EM_error(a->pos, "undefined function %s", S_name(a->u.call.func));
			return expTy(NULL, Ty_Int());
		}
		A_expList args = a->u.call.args;
		Ty_tyList formals = func->u.fun.formals;
		Ty_ty result = func->u.fun.result;
		Tr_expList TrexpList = explistSame(venv, tenv, args, formals, a, level, label);

		Tr_exp Trexp = Tr_callExp(func->u.fun.label, TrexpList, level, func->u.fun.level);
		return expTy(Trexp, actual_ty(result));
	}
	case A_opExp:
	{
		struct expty left = transExp(venv, tenv, a->u.op.left, level, label);
		struct expty right = transExp(venv, tenv, a->u.op.right, level, label);
		A_oper oper = a->u.op.oper;

		if (oper == A_plusOp || oper == A_minusOp || oper == A_timesOp || oper == A_divideOp)
		{
			if (actual_ty(left.ty)->kind != Ty_int || actual_ty(right.ty)->kind != Ty_int)
			{
				EM_error(a->u.op.left->pos, "integer required");
				return expTy(NULL, Ty_Int());
			}
		}
		else
		{
			if (actual_ty(left.ty) != actual_ty(right.ty))
			{
				EM_error(a->u.op.left->pos, "same type required");
				return expTy(NULL, Ty_Int());
			}
		}
		return expTy(Tr_opExp(oper, left.exp, right.exp), Ty_Int());
	}
	case A_recordExp:
	{
		Ty_ty type = S_look(tenv, a->u.record.typ);
		type = actual_ty(type);

		if (!type || type->kind != Ty_record)
		{
			EM_error(a->pos, "undefined type %s", S_name(a->u.record.typ));
			return expTy(NULL, Ty_Int());
		}

		A_efieldList tmp = a->u.record.fields;
		Ty_fieldList list = type->u.record;
		Tr_expList Trexplist = fieldlistSame(venv, tenv, tmp, list, a, level, label);
		int size = 0;
		while(Trexplist)
		{
			++size;
			Trexplist = Trexplist->tail;
		}
		return expTy(Tr_recordExp(size, Trexplist), type);
	}
	case A_seqExp:
	{
		A_expList list = a->u.seq;
		if (!list)
		{
			return expTy(Tr_nilExp(), Ty_Void());
		}
		else
		{
			struct expty result;
			for (; list; list = list->tail)
			{
				result = transExp(venv, tenv, list->head, level, label);
			}
			return result;
		}
	}
	case A_assignExp:
	{
		A_var tmp = a->u.assign.var;
		if (tmp->kind == A_simpleVar)
		{
			E_enventry e = S_look(venv, tmp->u.simple);
			if (e->readonly)
			{
				EM_error(a->pos, "loop variable can't be assigned");
				return expTy(NULL, Ty_Int());
			}
		}

		struct expty v = transVar(venv, tenv, tmp, level, label);
		A_exp t = a->u.assign.exp;
		struct expty exp = transExp(venv, tenv, t, level, label);

		if (actual_ty(v.ty) != actual_ty(exp.ty))
		{
			EM_error(a->pos, "unmatched assign exp");
			return expTy(NULL, Ty_Int());
		}
		return expTy(Tr_assignExp(v.exp, exp.exp), Ty_Void());
	}
	case A_ifExp:
	{
		A_exp condition = a->u.iff.test;

		struct expty conditionTy = transExp(venv, tenv, condition, level, label);
		if (actual_ty(conditionTy.ty)->kind != Ty_int)
		{
			EM_error(a->pos, "the condition exp should be an int expression");
			return expTy(NULL, Ty_Int());
		}

		A_exp thenExp = a->u.iff.then;
		A_exp elseExp = a->u.iff.elsee;
		struct expty thenExpTy = transExp(venv, tenv, thenExp, level, label);
		struct expty elseExpTy = transExp(venv, tenv, elseExp, level, label);
		if (actual_ty(elseExpTy.ty)->kind == Ty_nil)
		{
			if (actual_ty(thenExpTy.ty)->kind != Ty_void)
			{
				EM_error(a->pos, "if-then exp's body must produce no value");
				return expTy(NULL, Ty_Int());
			}
		}
		else
		{
			if (actual_ty(elseExpTy.ty) != actual_ty(thenExpTy.ty))
			{
				EM_error(a->pos, "then exp and else exp type mismatch");
				return expTy(NULL, Ty_Int());
			}
			return expTy(Tr_ifExp(conditionTy.exp, thenExpTy.exp, elseExpTy.exp), actual_ty(elseExpTy.ty));
		}
	}
	case A_whileExp:
	{
		A_exp condition = a->u.whilee.test;

		struct expty conditionTy = transExp(venv, tenv, condition, level, label);
		if (actual_ty(conditionTy.ty)->kind != Ty_int)
		{
			EM_error(a->pos, "the condition exp should be an int expression");
			return expTy(NULL, Ty_Int());
		}

		Temp_label finish = Temp_newlabel();
		A_exp body = a->u.whilee.body;
		struct expty bodyTy = transExp(venv, tenv, body, level, finish);
		if (actual_ty(bodyTy.ty)->kind != Ty_void)
		{
			EM_error(a->pos, "while body must produce no value");
			return expTy(NULL, Ty_Int());
		}

		return expTy(Tr_whileExp(conditionTy.exp, bodyTy.exp, finish), Ty_Void());
	}
	case A_forExp:
	{
		Temp_label finish = Temp_newlabel();
		A_exp initial = a->u.forr.lo;
		A_exp end = a->u.forr.hi;

		struct expty initialTy = transExp(venv, tenv, initial, level, label);
		struct expty endTy = transExp(venv, tenv, end, level, label);

		if (actual_ty(initialTy.ty)->kind != Ty_int || actual_ty(endTy.ty)->kind != Ty_int)
		{
			EM_error(a->pos, "for exp's range type is not integer");
			return expTy(NULL, Ty_Int());
		}

		Tr_access access = Tr_allocLocal(level, a->u.forr.escape);
		S_beginScope(venv);
		S_enter(venv, a->u.forr.var, E_ROVarEntry(access, Ty_Int()));

		A_exp body = a->u.forr.body;
		struct expty bodyTy = transExp(venv, tenv, body, level, finish);
		S_endScope(venv);
		if(actual_ty(bodyTy.ty)->kind != Ty_void)
		{
			EM_error(a->pos, "body exp's type is not void");
			return expTy(NULL, Ty_Int());
		}
		return expTy(Tr_forExp(access, initialTy.exp, endTy.exp, bodyTy.exp, finish), Ty_Void());
	}
	case A_breakExp:
	{
		return expTy(Tr_breakExp(label), Ty_Void());
	}
	case A_letExp:
	{
		S_beginScope(venv);
		S_beginScope(tenv);
		A_decList tmp = a->u.let.decs;
		for (; tmp; tmp = tmp->tail)
		{
			transDec(venv, tenv, tmp->head, level, label);
		}
		struct expty body = transExp(venv, tenv, a->u.let.body, level, label);
		S_endScope(tenv);
		S_endScope(venv);
		EM_error("reach here let exp\n");
		return body;
	}
	case A_arrayExp:
	{
		Ty_ty tmp = S_look(tenv, a->u.array.typ);
		tmp = actual_ty(tmp);
		if (!tmp || tmp->kind != Ty_array)
		{
			EM_error(a->pos, "undefined array %s", S_name(a->u.array.typ));
			return expTy(NULL, Ty_Int());
		}
		A_exp size = a->u.array.size;
		struct expty sizeTy = transExp(venv, tenv, size, level, label);
		if (actual_ty(sizeTy.ty)->kind != Ty_int)
		{
			EM_error(a->pos, "the size should be an integer");
			return expTy(NULL, Ty_Int());
		}

		A_exp initial = a->u.array.init;
		struct expty initialTy = transExp(venv, tenv, initial, level, label);
		if (actual_ty(initialTy.ty) != actual_ty(tmp->u.array))
		{
			EM_error(a->pos, "type mismatch");
			return expTy(NULL, Ty_Int());
		}
		return expTy(Tr_arrayExp(sizeTy.exp, initialTy.exp), actual_ty(tmp));
	}
	}
}

Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params)
{
	if (params == NULL)
	{
		return NULL;
	}

	Ty_ty first = S_look(tenv, params->head->typ);
	if (!first)
	{
		EM_error(params->head->pos, "undefined type %s", S_name(params->head->typ));
		first = Ty_Int();
	}
	Ty_tyList tail = makeFormalTyList(tenv, params->tail);

	return Ty_TyList(first, tail);
}

U_boolList functionParamEscape(A_fieldList params)
{
	if(params)
	{
		return U_BoolList(params->head->escape, functionParamEscape(params->tail));
	}
	else
	{
		return NULL;
	}
}

struct expty transDec(S_table venv, S_table tenv, A_dec d, Tr_level level, Temp_label label)
{
	switch (d->kind)
	{
	case A_functionDec:
	{
		A_fundecList list = d->u.function;
		Ty_ty result;
		for (; list; list = list->tail)
		{
			A_fundec tmp = list->head;
			if (S_look(venv, tmp->name))
			{
				EM_error(d->pos, "two functions have the same name");
				continue;
			}
			if (!tmp->result)
			{
				result = Ty_Void();
			}
			else
			{
				result = S_look(tenv, tmp->result);
				if (!result)
				{
					EM_error(d->pos, "wrong return type");
					result = Ty_Void();
				}
			}
			Ty_tyList formals = makeFormalTyList(tenv, tmp->params);
			Temp_label newLable = Temp_newlabel();
			U_boolList escapes = functionParamEscape(tmp->params);
			S_enter(venv, tmp->name, E_FunEntry(Tr_newLevel(level, newLable, escapes), newLable, formals, result));
		}
		list = d->u.function;
		for (; list; list = list->tail)
		{
			A_fundec tmp = list->head;
			E_enventry e = S_look(venv, tmp->name);
			Ty_tyList formals = e->u.fun.formals;
			S_beginScope(venv);
			A_fieldList fieldlist = tmp->params;
			Tr_accessList accesses = Tr_formals(e->u.fun.level);
			for (; fieldlist && formals && accesses; fieldlist = fieldlist->tail, formals = formals->tail, accesses = accesses->tail)
			{
				S_enter(venv, fieldlist->head->name, E_VarEntry(accesses->head, formals->head));
			}

			struct expty returnType = transExp(venv, tenv, tmp->body, e->u.fun.level, label);
			if (actual_ty(returnType.ty) != actual_ty(e->u.fun.result))
			{
				if (actual_ty(e->u.fun.result)->kind == Ty_void)
				{
					EM_error(d->pos, "procedure returns value");
				}
				else
				{
					if (!(actual_ty(returnType.ty)->kind == Ty_nil && actual_ty(e->u.fun.result)->kind == Ty_record))
					{
						EM_error(d->pos, "wrong return type");
					}
				}
			}
			S_endScope(venv);
		}
		return expTy(NULL, Ty_Int());
	}
	case A_varDec:
	{
		struct expty initial = transExp(venv, tenv, d->u.var.init, level, label);
		Tr_access access = Tr_allocLocal(level, d->u.var.escape);
		if (d->u.var.typ)
		{
			Ty_ty type = S_look(tenv, d->u.var.typ);
			if (!type)
			{
				EM_error(d->pos, "undefined type %s", S_name(d->u.var.typ));
				return expTy(NULL, Ty_Int());
			}
			if (actual_ty(type) != actual_ty(initial.ty))
			{
				if (!(actual_ty(type)->kind == Ty_record && actual_ty(initial.ty)->kind == Ty_nil))
				{
					EM_error(d->pos, "type mismatch");
					return expTy(NULL, Ty_Int());
				}
			}
			S_enter(venv, d->u.var.var, E_VarEntry(access, type));
		}
		else
		{
			if(actual_ty(initial.ty)->kind == Ty_nil)
			{
				EM_error(d->pos, "init should not be nil without type specified");
				return expTy(NULL, Ty_Int());
			}
			S_enter(venv, d->u.var.var, E_VarEntry(access, initial.ty));
		}
		return expTy(Tr_assignExp(Tr_simpleVar(access, level), initial.exp), Ty_Int());
	}
	case A_typeDec:
	{
		A_nametyList tylist = d->u.type;
		for(; tylist; tylist = tylist->tail)
		{
			A_namety tmp = tylist->head;
			if(S_look(tenv, tmp->name))
			{
				EM_error(d->pos, "two types have the same name");
				return expTy(NULL, Ty_Int());
			}
			else
			{
				S_enter(tenv, tmp->name, Ty_Name(tmp->name, transTy(tenv, tmp->ty)));
			}
		}

		tylist = d->u.type;
		for(; tylist; tylist = tylist->tail)
		{
			A_namety tmp = tylist->head;
			Ty_ty type = S_look(tenv, tmp->name);
			type->u.name.ty = transTy(tenv, tmp->ty);
		}

		tylist = d->u.type;
		for(; tylist; tylist = tylist->tail)
		{
			A_namety tmp = tylist->head;
			Ty_ty type = S_look(tenv, tmp->name);
			Ty_ty t = type->u.name.ty;
			for (; t && t->kind == Ty_name; t = t->u.name.ty)
			{
				if(t == type)
				{
					EM_error(d->pos, "illegal type cycle");
					type->u.name.ty = Ty_Int();
					return expTy(NULL, Ty_Int());
				}
			}
		}
		return expTy(NULL, Ty_Int());
	}
	}
}

Ty_fieldList makeRecordTyList(S_table tenv, A_fieldList params)
{
	if (params == NULL)
	{
		return NULL;
	}

	Ty_ty first = S_look(tenv, params->head->typ);
	if (!first)
	{
		EM_error(params->head->pos, "undefined type %s", S_name(params->head->typ));
		first = Ty_Int();
	}
	Ty_field firstfield = Ty_Field(params->head->name, first);
	Ty_fieldList tail = makeRecordTyList(tenv, params->tail);

	return Ty_FieldList(firstfield, tail);
}

Ty_ty transTy(S_table tenv, A_ty a)
{
	switch (a->kind)
	{
	case A_nameTy:
	{
		Ty_ty type = S_look(tenv, a->u.name);
		if (!type)
		{
			EM_error(a->pos, "undefined type %s", S_name(a->u.name));
			return Ty_Int();
		}
		return Ty_Name(a->u.name, type);
	}
	case A_recordTy:
	{
		A_fieldList lists = a->u.record;
		return Ty_Record(makeRecordTyList(tenv, lists));
	}
	case A_arrayTy:
	{
		Ty_ty type = S_look(tenv, a->u.array);
		if (!type)
		{
			EM_error(a->pos, "undefined type %s", S_name(a->u.array));
			return Ty_Int();
		}
		return Ty_Array(type);
	}
	}
}

F_fragList SEM_transProg(A_exp exp)
{
	Temp_label label = Temp_newlabel();
	Tr_level mainn = Tr_newLevel(Tr_outermost(), label, NULL);
	E_enventry fun = E_FunEntry(mainn, label, NULL, Ty_Void());

	struct expty body = transExp(E_base_venv(), E_base_tenv(), exp, mainn, NULL);

	Tr_procEntryExit1(fun->u.fun.level, body.exp, NULL);
	return Tr_getResult();
}
