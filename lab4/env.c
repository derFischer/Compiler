#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "env.h"

/*Lab4: Your implementation of lab4*/

E_enventry E_VarEntry(Ty_ty ty)
{
	E_enventry tmp = checked_malloc(sizeof(*tmp));
	tmp->kind = E_varEntry;
	tmp->u.var.ty = ty;
	tmp->RO = false;
	return tmp;
}

E_enventry RO_E_VarEntry(Ty_ty ty)
{
	E_enventry tmp = checked_malloc(sizeof(*tmp));
	tmp->kind = E_varEntry;
	tmp->u.var.ty = ty;
	tmp->RO = true;
	return tmp;
}

E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result)
{
	E_enventry tmp = checked_malloc(sizeof(*tmp));
	tmp->kind = E_funEntry;
	tmp->u.fun.formals = formals;
	tmp->u.fun.result = result;
	tmp->RO = false;
	return tmp;
}

S_table E_base_tenv(void)
{
	S_table baseTenv = S_empty();
	S_symbol typeInt = S_Symbol("int");
	S_symbol typeString = S_Symbol("string");

	S_enter(baseTenv, typeInt, Ty_Int());
	S_enter(baseTenv, typeString, Ty_String());

	return baseTenv;
}

S_table E_base_venv(void)
{
	S_table baseVenv = S_empty();
	return baseVenv;
}
