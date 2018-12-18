#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "temp.h"
#include "errormsg.h"
#include "tree.h"
#include "assem.h"
#include "frame.h"
#include "codegen.h"
#include "table.h"
#define INSTLENGTH 200
AS_instr moveReg(Temp_temp src, Temp_temp dst)
{
    return AS_Move("movq `s0, `d0", L(dst, NULL), L(src, NULL), NULL);
}

static Temp_temp opCode(string op, T_exp left, T_exp right)
{
    Temp_temp result = Temp_newtemp();
    if (left->kind == T_CONST)
    {
        AS_inst prepareResult = moveReg(munchExp(right), result);
        char inst[INSTLENGTH];
        sprintf("%s $%d, `d0", op, left->u.CONST);
        AS_inst calOper = AS_Oper(String(inst), L(result, NULL), NULL, NULL);
    }
    else if (right->kind == T_CONST)
    {
        AS_inst prepareResult = moveReg(munchExp(left), result);
        char inst[INSTLENGTH];
        sprintf("%s $%d, `d0", op, right->u.CONST);
        AS_inst calOper = AS_Oper(String(inst), L(result, NULL), NULL, NULL);
    }
    else
    {
        AS_inst prepareResult = moveReg(munchExp(left), result);
        char inst[INSTLENGTH];
        sprintf("%s `s0, `d0", op);
        AS_inst calOper = AS_Oper(String(inst), L(munchExp(right), NULL), L(result, NULL), NULL);
    }
    return result;
}

static Temp_temp munchExp(T_exp e)
{
    switch (e->kind)
    {
    case T_BINOP:
    {
        string oper;
        switch (e->u.BINOP.op)
        {
        case T_plus:
        {
            oper = "addq";
            break;
        }
        case T_minus:
        {
            oper = "subq";
            break;
        }
        case T_mul:
        {
            oper = "imulq";
            break;
        }
        case T_and:
        {
            oper = "addq";
            break;
        }
        case T_or:
        {
            oper = "addq";
            break;
        }
        case T_lshift:
        {
            oper = "addq";
            break;
        }
        case T_rshift:
        {
            oper = "addq";
            break;
        }
        case T_arshift:
        {
            oper = "addq";
            break;
        }
        case T_xor:
        {
            oper = "addq";
            break;
        }
        }
        if (e->u.BINOP.op != T_div)
        {
            return opCode(oper, e->u.BINOP.left, e->u.BINOP.right);
        }
        else
        {
            Temp_temp result = Temp_newtemp();
            Temp_temp left = munchExp(left);
            Temp_temp right = munchExp(right);
            AS_instr prepareResult1 = moveReg(left, F_RAX());
            AS_instr prepareResult2 = AS_Oper("cqto", L(F_RDX(), NULL), NULL, NULL);
            AS_instr calOper = AS_Oper("idivq `s0", L(F_RAX(), L(F_RDX(), NULL)), L(L(F_RDX(), L(F_RAX(), L(right, NULL)))), NULL);
            AS_instr movRes = moveReg(F_RAX(), result);
            emit(prepareResult1);
            emit(prepareResult2);
            emit(calOper);
            emit(movRes);
            return result;
        }
    }
    case T_MEM:
    {
        Temp_temp dst = Temp_newtemp();
        T_exp addr = e->u.MEM;
        switch (addr->u.kind)
        {
        case T_CONST:
        {
            char inst[INSTLENGTH];
            sprintf("movq %d, `d0", addr->u.CONST);
            emit(AS_Move(String(inst), L(dst, NULL), NULL, NULL));
            break;
        }
        case T_BINOP:
        {
            T_exp left = addr->u.BINOP.left;
            T_exp right = addr->u.BINOP.right;
            if (left->u.kind == T_CONST && right->u.kind == T_TEMP)
            {
                char inst[INSTLENGTH];
                sprintf("movq %d(`s0), `d0", left->u.CONST);
                emit(AS_Move(String(inst), L(munchExp(right), NULL), L(dst, NULL)));
            }
            else if (right->u.kind == T_CONST && left->u.kind == T_TEMP)
            {
                char inst[INSTLENGTH];
                sprintf("movq %d(`s0), `d0", right->u.CONST);
                emit(AS_Move(String(inst), L(munchExp(left), NULL), L(dst, NULL)));
            }
            else if (left->u.kind == T_TEMP && right->u.kind == T_TEMP)
            {
                char inst[INSTLENGTH];
                sprintf("movq (`s0,`s1), `d0");
                emit(AS_Move(String(inst), L(munchExp(left), L(munchExp(right), NULL)), L(dst, NULL)));
            }
            else
            {
                Temp_temp src = munchExp(addr);
                emit(AS_Move("movq `s0, `d0", L(src, NULL), L(dst, NULL)));
            }
            break;
        }
        default:
        {
            Temp_temp src = munchExp(addr);
            emit(AS_Move("movq `s0, `d0", L(src, NULL), L(dst, NULL)));
        }
        }
        return dst;
    }
    case T_TEMP:
    {
        return e->u.TEMP;
    }
    case T_ESEQ:
    {
        printf("eseq should be eliminated\n");
        assert(0);
    }
    case T_NAME:
    {
        
    }
    }
}

//Lab 6: put your code here
AS_instrList F_codegen(F_frame f, T_stmList stmList)
{
}
