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
        sprintf(inst, "%s $%d, `d0", op, left->u.CONST);
        AS_inst calOper = AS_Oper(String(inst), L(result, NULL), NULL, NULL);
    }
    else if (right->kind == T_CONST)
    {
        AS_inst prepareResult = moveReg(munchExp(left), result);
        char inst[INSTLENGTH];
        sprintf(inst, "%s $%d, `d0", op, right->u.CONST);
        AS_inst calOper = AS_Oper(String(inst), L(result, NULL), NULL, NULL);
    }
    else
    {
        AS_inst prepareResult = moveReg(munchExp(left), result);
        char inst[INSTLENGTH];
        sprintf(inst, "%s `s0, `d0", op);
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
            sprintf(inst, "movq %d, `d0", addr->u.CONST);
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
                sprintf(inst, "movq %d(`s0), `d0", left->u.CONST);
                emit(AS_Move(String(inst), L(dst, NULL), L(munchExp(right), NULL)));
            }
            else if (right->u.kind == T_CONST && left->u.kind == T_TEMP)
            {
                char inst[INSTLENGTH];
                sprintf(inst, "movq %d(`s0), `d0", right->u.CONST);
                emit(AS_Move(String(inst), L(dst, NULL), L(munchExp(left), NULL)));
            }
            else if (left->u.kind == T_TEMP && right->u.kind == T_TEMP)
            {
                char inst[INSTLENGTH];
                sprintf(inst, "movq (`s0,`s1), `d0");
                emit(AS_Move(String(inst), L(dst, NULL), L(munchExp(left), L(munchExp(right), NULL))));
            }
            else
            {
                Temp_temp src = munchExp(addr);
                emit(AS_Move("movq (`s0), `d0", L(dst, NULL), L(src, NULL)));
            }
            break;
        }
        default:
        {
            Temp_temp src = munchExp(addr);
            emit(AS_Move("movq (`s0), `d0", L(dst, NULL), L(src, NULL)));
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
        Temp_temp dst = Temp_newtemp();
        char inst[INSTLENGTH];
        sprintf(inst, "leaq %s, `d0", Temp_labelstring(e->u.NAME));
        emit(AS_Oper(String(inst), L(dst, NULL), NULL, NULL));
        return dst;
    }
    case T_CONST:
    {
        Temp_temp dst = Temp_newtemp();
        char inst[INSTLENGTH];
        sprintf(inst, "movq $%d, `d0", e->u.CONST);
        emit(AS_Move(String(inst), L(dst, NULL), NULL, NULL));
        return dst;
    }
    case T_CALL:
    {
        T_exp fun = e->u.CALL.fun;
        T_expList args = e->u.CALL.args;

    }
    }
}

static void passArgs(T_expList args)
{
    T_exp staticLink = args->head;
    Temp_temp slReg = munchExp(staticLink);
    emit(AS_Oper("push `s0", Temp_TempList(F_RSP(), NULL), Temp_TempList(slReg, NULL), NULL));
    int index = 1;
    while(args && index <= 6)
    {
        T_exp arg = args->head;
        Temp_temp argReg = munchExp(arg);
        
    }
}

static void munchStm(T_stm s)
{
    switch (s->kind)
    {
    case T_SEQ:
    {
        printf("seq should be eliminated\n");
        assert(0);
    }
    case T_LABEL:
    {
        emit(AS_Label(Temp_labelstring(s->u.LABEL), s->u.LABEL));
        return;
    }
    case T_JUMP:
    {
        emit(AS_Oper("jmp `j0", NULL, NULL, AS_Targets(s->u.JUMP.jumps)));
        return;
    }
    case T_CJUMP:
    {
        emit(AS_Oper("cmpq `s0, `s1", NULL, L(munchExp(s->u.CJUMP.right), L(munchExp(s->u.CJUMP.left), NULL)), NULL));
        switch (s->u.CJUMP.op)
        {
        case T_eq:
        {
            emit(AS_Oper("je `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
        }
        case T_ne:
        {
            emit(AS_Oper("jne `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
        }
        case T_lt:
        {
            emit(AS_Oper("jlt `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
        }
        case T_gt:
        {
            emit(AS_Oper("jgt `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
        }
        case T_le:
        {
            emit(AS_Oper("jle `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
        }
        case T_ge:
        {
            emit(AS_Oper("jge `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
        }
        case T_ult:
        {
            emit(AS_Oper("jb `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
        }
        case T_ule:
        {
            emit(AS_Oper("jbe `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
        }
        case T_ugt:
        {
            emit(AS_Oper("ja `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
        }
        case T_uge:
        {
            emit(AS_Oper("jae `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
        }
        }
        return;
    }
    case T_MOVE:
    {
        Temp_temp src = munchExp(s->u.MOVE.src);
        T_exp dst = s->u.MOVE.dst;
        if (dst->kind == T_MEM)
        {
            T_exp addr = dst->u.MEM;
            switch (addr->u.kind)
            {
            case T_CONST:
            {
                char inst[INSTLENGTH];
                sprintf(inst, "movq `s0, %d", addr->u.CONST);
                emit(AS_Move(String(inst), NULL, L(src, NULL), NULL));
                break;
            }
            case T_BINOP:
            {
                T_exp left = addr->u.BINOP.left;
                T_exp right = addr->u.BINOP.right;
                if (left->u.kind == T_CONST && right->u.kind == T_TEMP)
                {
                    char inst[INSTLENGTH];
                    sprintf(inst, "movq `s0, %d(`s1)", left->u.CONST);
                    emit(AS_Move(String(inst), NULL, L(src, L(munch(right), NULL)), NULL)));
                }
                else if (right->u.kind == T_CONST && left->u.kind == T_TEMP)
                {
                    char inst[INSTLENGTH];
                    sprintf(inst, "movq `s0, %d(`s1)", right->u.CONST);
                    emit(AS_Move(String(inst), NULL, L(src, L(munch(left), NULL)), NULL)));
                }
                else if (left->u.kind == T_TEMP && right->u.kind == T_TEMP)
                {
                    char inst[INSTLENGTH];
                    sprintf(inst, "movq `s0, (`s1,`s2)");
                    emit(AS_Move(String(inst), NULL, L(src, L(munch(left), L(munch(right), NULL))), NULL));
                }
                else
                {
                    Temp_temp address = munchExp(addr);
                    emit(AS_Move("movq `s0, (`s1)", NULL, L(src, L(address, NULL))));
                }
                break;
            }
            default:
            {
                Temp_temp src = munchExp(addr);
                emit(AS_Move("movq `s0, (`s1)", NULL, L(src, L(address, NULL))));
            }
            }
        }
        else if (dst->kind == T_Temp)
        {
            emit(AS_Move("movq `s0, `d0", L(munchExp(dst), NULL), L(src, NULL)));
        }
        else
        {
            printf("the move destination should be temp or mem\n");
            assert(0);
        }
        return;
    }
    case T_EXP:
    {
        munchExp(s->u.EXP);
        return;
    }
    default:
    {
        printf("no such statement, please debug\n");
        assert(0);
    }
    }
}

//Lab 6: put your code here
AS_instrList F_codegen(F_frame f, T_stmList stmList)
{
}
