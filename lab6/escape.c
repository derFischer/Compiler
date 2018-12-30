#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"
#include "types.h"
#include "env.h"
#include "escape.h"
#include "helper.h"

#define TRUE 1
#define FALSE 0
static void traverseExp(S_table env, int depth, A_exp e);
static void traverseDec(S_table env, int depth, A_dec d);
static void traverseVar(S_table env, int depth, A_var v);


//constructor
escapeEntry EscapeEntry(int depth, bool *escape)
{
    escapeEntry entry = malloc(sizeof(*entry));
    entry->depth = depth;
    entry->escape = escape;
    return entry;
}

static void traverseExp(S_table env, int depth, A_exp e)
{
    switch(e->kind)
    {
        case A_varExp:
        {
            return traverseVar(env, depth, e->u.var);
        }
        case A_nilExp:
        case A_intExp:
        case A_stringExp:
        case A_breakExp:
        return;
        case A_callExp:
        {
            A_expList args = get_callexp_args(e);
            for(; args; args = args->tail)
            {
                traverseExp(env, depth, args->head);
            }
            return;
        }
        case A_opExp:
        {
            traverseExp(env, depth, get_opexp_left(e));
            traverseExp(env, depth, get_opexp_right(e));
            return;
        }
        case A_recordExp:
        {
            A_efieldList fields = get_recordexp_fields(e);
            for(; fields; fields = fields->tail)
            {
                traverseExp(env, depth, fields->head->exp);
            }
            return;
        }
        case A_seqExp:
        {
            A_expList seq = get_seqexp_seq(e);
            for(; seq; seq = seq->tail)
            {
                traverseExp(env, depth, seq->head);
            }
            return;
        }
        case A_assignExp:
        {
            traverseVar(env, depth, get_assexp_var(e));
            traverseExp(env, depth, get_assexp_exp(e));
            return;
        }
        case A_ifExp:
        {
            traverseExp(env, depth, get_ifexp_test(e));
            traverseExp(env, depth, get_ifexp_then(e));
            traverseExp(env, depth, get_ifexp_else(e));
            return;
        }
        case A_whileExp:
        {
            traverseExp(env, depth, get_whileexp_test(e));
            traverseExp(env, depth, get_whileexp_body(e));
            return;
        }
        case A_forExp:
        {
            traverseExp(env, depth, get_forexp_lo(e));
            traverseExp(env, depth, get_forexp_hi(e));
            S_beginScope(env);
            S_enter(env, get_forexp_var(e), EscapeEntry(depth, &(e->u.forr.escape)));
            e->u.forr.escape = FALSE;
            traverseExp(env, depth, get_forexp_body(e));
            S_endScope(env);
            return;
        }
        case A_letExp:
        {
            S_beginScope(env);
            A_decList decs = get_letexp_decs(e);
            for(; decs; decs = decs->tail)
            {
                traverseDec(env, depth, decs->head);
            }
            traverseExp(env, depth, get_letexp_body(e));
            S_endScope(env);
            return;
        }
        case A_arrayExp:
        {
            traverseExp(env, depth, get_arrayexp_size(e));
            traverseExp(env, depth, get_arrayexp_init(e));
            return;
        }
    }
}

static void traverseDec(S_table env, int depth, A_dec d)
{
    switch(d->kind)
    {
        case A_functionDec:
        {
            A_fundecList function = get_funcdec_list(d);
            for(; function; function = function->tail)
            {
                S_beginScope(env);
                A_fieldList params = function->head->params;
                for(; params; params = params->tail)
                {
                    S_enter(env, params->head->name, EscapeEntry(depth + 1, &(params->head->escape)));
                    params->head->escape = FALSE;
                }
                traverseExp(env, depth + 1, function->head->body);
                S_endScope(env);
            }
            return;
        }
        case A_varDec:
        {
            S_enter(env, get_vardec_var(d), EscapeEntry(depth, &(d->u.var.escape)));
            d->u.var.escape = FALSE;   
            traverseExp(env, depth, get_vardec_init(d));
            return;
        }
    }
}

static void traverseVar(S_table env, int depth, A_var v)
{
    switch(v->kind)
    {
        case A_simpleVar:
        {
            printf("traverse simplevar\n");
            escapeEntry ee = S_look(env, get_simplevar_sym(v));
            if(ee && ee->depth < depth)
            {
                printf("analysis escape find an escape v\n");
                *(ee->escape) = 1;
            }
            return;
        }
        case A_fieldVar:
        {
            traverseVar(env, depth, get_fieldvar_var(v));
            return;
        }
        case A_subscriptVar:
        {
            traverseVar(env, depth, get_subvar_var(v));
            traverseExp(env, depth, get_subvar_exp(v));
            return;
        }
    }
}

void Esc_findEscape(A_exp exp)
{
    traverseExp(S_empty(), 0, exp);
}