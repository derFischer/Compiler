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

static AS_instrList iList = NULL, last = NULL;

static Temp_tempList passArgs(T_expList args);
static Temp_temp munchExp(T_exp e);
static void munchStm(T_stm s);
int calArgsStackSpace(T_expList args);

Temp_tempList L(Temp_temp head, Temp_tempList tail)
{
    return Temp_TempList(head, tail);
}

static void emit(AS_instr inst)
{
    if (last != NULL)
    {
        last->tail = AS_InstrList(inst, NULL);
        last = last->tail;
    }
    else
    {
        last = AS_InstrList(inst, NULL);
        iList = last;
    }
}

AS_instr moveReg(Temp_temp src, Temp_temp dst)
{
    return AS_Move("movq `s0, `d0", L(dst, NULL), L(src, NULL));
}

static Temp_temp opCode(string op, T_exp left, T_exp right)
{
    Temp_temp result = Temp_newtemp();
    AS_instr calOper;
    AS_instr prepareResult;
    if (left->kind == T_CONST && !strstr(op, "subq"))
    {
        prepareResult = moveReg(munchExp(right), result);
        char inst[INSTLENGTH];
        sprintf(inst, "%s $%d, `d0", op, left->u.CONST);
        calOper = AS_Oper(String(inst), L(result, NULL), L(result, NULL), NULL);
    }
    else if (right->kind == T_CONST)
    {
        prepareResult = moveReg(munchExp(left), result);
        char inst[INSTLENGTH];
        sprintf(inst, "%s $%d, `d0", op, right->u.CONST);
        calOper = AS_Oper(String(inst), L(result, NULL), L(result, NULL), NULL);
    }
    else
    {
        prepareResult = moveReg(munchExp(left), result);
        char inst[INSTLENGTH];
        sprintf(inst, "%s `s0, `d0", op);
        calOper = AS_Oper(String(inst), L(result, NULL), L(munchExp(right), L(result, NULL)), NULL);
    }
    emit(prepareResult);
    emit(calOper);
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
            Temp_temp left = munchExp(e->u.BINOP.left);
            Temp_temp right = munchExp(e->u.BINOP.right);
            AS_instr prepareResult1 = moveReg(left, F_RAX());
            AS_instr prepareResult2 = AS_Oper("cqto", L(F_RDX(), NULL), NULL, NULL);
            AS_instr calOper = AS_Oper("idivq `s0", L(F_RAX(), L(F_RDX(), NULL)), L(right, L(F_RAX(), NULL)), NULL);
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
        switch (addr->kind)
        {
        case T_CONST:
        {
            char inst[INSTLENGTH];
            sprintf(inst, "movq %d, `d0", addr->u.CONST);
            emit(AS_Oper(String(inst), L(dst, NULL), NULL, NULL));
            break;
        }
        case T_BINOP:
        {
            T_exp left = addr->u.BINOP.left;
            T_exp right = addr->u.BINOP.right;
            if (left->kind == T_CONST && right->kind == T_TEMP)
            {
                char inst[INSTLENGTH];
                sprintf(inst, "movq %d(`s0), `d0", left->u.CONST);
                emit(AS_Oper(String(inst), L(dst, NULL), L(munchExp(right), NULL), NULL));
            }
            else if (right->kind == T_CONST && left->kind == T_TEMP)
            {
                char inst[INSTLENGTH];
                sprintf(inst, "movq %d(`s0), `d0", right->u.CONST);
                emit(AS_Oper(String(inst), L(dst, NULL), L(munchExp(left), NULL), NULL));
            }
            else if (left->kind == T_TEMP && right->kind == T_TEMP)
            {
                char inst[INSTLENGTH];
                sprintf(inst, "movq (`s0,`s1), `d0");
                emit(AS_Oper(String(inst), L(dst, NULL), L(munchExp(left), L(munchExp(right), NULL)), NULL));
            }
            else
            {
                Temp_temp src = munchExp(addr);
                emit(AS_Oper("movq (`s0), `d0", L(dst, NULL), L(src, NULL), NULL));
            }
            break;
        }
        default:
        {
            Temp_temp src = munchExp(addr);
            emit(AS_Oper("movq (`s0), `d0", L(dst, NULL), L(src, NULL), NULL));
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
        emit(AS_Oper(String(inst), L(dst, NULL), NULL, NULL));
        return dst;
    }
    case T_CALL:
    {
        T_exp fun = e->u.CALL.fun;
        T_expList args = e->u.CALL.args;
        Temp_tempList argsRegs = passArgs(args);
        if (fun->kind != T_NAME)
        {
            printf("call fun should be a name label\n");
            assert(0);
        }
        char inst[INSTLENGTH];
        sprintf(inst, "call %s", Temp_labelstring(fun->u.NAME));
        emit(AS_Oper(String(inst), Temp_TempList(F_RAX(), F_callersaves()), argsRegs, NULL));
        if (calArgsStackSpace(args) != 0)
        {
            char inst2[INSTLENGTH];
            sprintf(inst2, "addq $%d, `d0", calArgsStackSpace(args) * WORDSIZE);
            emit(AS_Oper(String(inst2), Temp_TempList(F_RSP(), NULL), Temp_TempList(F_RSP(), NULL), NULL));
        }
        return F_RV();
    }
    }
}

int calArgsStackSpace(T_expList args)
{
    int num = 0;
    while (args)
    {
        num++;
        args = args->tail;
    }
    if (num > 6)
    {
        num -= 6;
    }
    else
    {
        num = 0;
    }
    return num;
}

static Temp_tempList passArgs(T_expList args)
{
    Temp_tempList result = NULL;
    int index = 1;
    while (args && index <= 6)
    {
        T_exp arg = args->head;
        Temp_temp argReg = munchExp(arg);
        emit(AS_Move("movq `s0, `d0", Temp_TempList(F_argsReg(index), NULL), Temp_TempList(argReg, NULL)));
        args = args->tail;
        index++;
        result = Temp_TempList(F_argsReg(index), result);
    }
    T_expList tmp = args;
    if (tmp)
    {
        int stackArgNum = 0;
        while (tmp)
        {
            ++stackArgNum;
            tmp = tmp->tail;
        }
        char inst[INSTLENGTH];
        sprintf(inst, "subq $%d, `d0", stackArgNum * WORDSIZE);
        emit(AS_Oper(String(inst), Temp_TempList(F_RSP(), NULL), NULL, NULL));
    }
    while (args)
    {
        T_exp arg = args->head;
        Temp_temp argReg = munchExp(arg);
        char inst[INSTLENGTH];
        sprintf(inst, "movq `s0, %d(`s1)", (index - 6) * WORDSIZE);
        emit(AS_Oper(String(inst), NULL, Temp_TempList(argReg, Temp_TempList(F_RSP(), NULL)), NULL));
        args = args->tail;
        index++;
    }
    return result;
}

static void munchStm(T_stm s)
{
    printf("stm kind %d\n", s->kind);
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
            break;
        }
        case T_ne:
        {
            emit(AS_Oper("jne `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
            break;
        }
        case T_lt:
        {
            emit(AS_Oper("jl `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
            break;
        }
        case T_gt:
        {
            emit(AS_Oper("jg `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
            break;
        }
        case T_le:
        {
            emit(AS_Oper("jle `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
            break;
        }
        case T_ge:
        {
            emit(AS_Oper("jge `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
            break;
        }
        case T_ult:
        {
            emit(AS_Oper("jb `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
            break;
        }
        case T_ule:
        {
            emit(AS_Oper("jbe `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
            break;
        }
        case T_ugt:
        {
            emit(AS_Oper("ja `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
            break;
        }
        case T_uge:
        {
            emit(AS_Oper("jae `j0", NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
            break;
        }
        }
        return;
    }
    case T_MOVE:
    {
        bool srcConst = FALSE;
        int srcConstNum = 0;
        T_exp srcExp = s->u.MOVE.src;
        if (srcExp->kind == T_CONST)
        {
            srcConst = TRUE;
            srcConstNum = srcExp->u.CONST;
        }
        T_exp dst = s->u.MOVE.dst;
        if (dst->kind == T_MEM)
        {
            Temp_temp src = munchExp(s->u.MOVE.src);
            T_exp addr = dst->u.MEM;
            switch (addr->kind)
            {
            case T_BINOP:
            {
                T_exp left = addr->u.BINOP.left;
                T_exp right = addr->u.BINOP.right;
                if (left->kind == T_CONST && right->kind == T_TEMP)
                {
                    char inst[INSTLENGTH];
                    sprintf(inst, "movq `s0, %d(`s1)", left->u.CONST);
                    emit(AS_Oper(String(inst), NULL, L(src, L(munchExp(right), NULL)), NULL));
                }
                else if (right->kind == T_CONST && left->kind == T_TEMP)
                {
                    char inst[INSTLENGTH];
                    sprintf(inst, "movq `s0, %d(`s1)", right->u.CONST);
                    emit(AS_Oper(String(inst), NULL, L(src, L(munchExp(left), NULL)), NULL));
                }
                else if (left->kind == T_TEMP && right->kind == T_TEMP)
                {
                    char inst[INSTLENGTH];
                    sprintf(inst, "movq `s0, (`s1,`s2)");
                    emit(AS_Oper(String(inst), NULL, L(src, L(munchExp(left), L(munchExp(right), NULL))), NULL));
                }
                else
                {
                    Temp_temp address = munchExp(addr);
                    emit(AS_Oper("movq `s0, (`s1)", NULL, L(src, L(address, NULL)), NULL));
                }
                break;
            }
            default:
            {
                Temp_temp address = munchExp(addr);
                if (srcConst)
                {
                    char inst[INSTLENGTH];
                    sprintf(inst, "movq $%d, (`s0)", srcConstNum);
                    emit(AS_Oper(String(inst), NULL, L(address, NULL), NULL));
                }
                else
                {
                    emit(AS_Oper("movq `s0, (`s1)", NULL, L(src, L(address, NULL)), NULL));
                }
            }
            }
        }
        else if (dst->kind == T_TEMP)
        {
            if (srcConst)
            {
                char inst[INSTLENGTH];
                sprintf(inst, "movq $%d, `d0", srcConstNum);
                Temp_temp dstt = munchExp(dst);
                emit(AS_Oper(String(inst), L(munchExp(dst), NULL), NULL, NULL));
            }
            else
            {
                Temp_temp src = munchExp(s->u.MOVE.src);
                Temp_temp dstt = munchExp(dst);
                emit(AS_Move("movq `s0, `d0", L(dstt, NULL), L(src, NULL)));
            }
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
    iList = NULL;
    last = NULL;
    char rbpConvert[INSTLENGTH];
    sprintf(rbpConvert, "%s_framesize", Temp_labelstring(F_name(f)));

    char frameSpace[INSTLENGTH];
    sprintf(frameSpace, "subq $%s, `d0", String(rbpConvert));
    emit(AS_Oper(String(frameSpace), Temp_TempList(F_RSP(), NULL), Temp_TempList(F_RSP(), NULL), NULL));

    F_accessList formals = F_formals(f);
    int index = 1;
    while (formals && index <= 6)
    {
        F_access access = formals->head;
        if (access->kind == inReg)
        {
            emit(AS_Move("movq `s0, `d0", Temp_TempList(access->u.reg, NULL), Temp_TempList(F_argsReg(index), NULL)));
        }
        else
        {
            int offset = access->u.offset;
            char moveArg[INSTLENGTH];
            sprintf(moveArg, "movq `s0, %d(`s1)", offset);
            emit(AS_Oper(String(moveArg), NULL, L(F_argsReg(index), L(F_RBP(), NULL)), NULL));
        }
        formals = formals->tail;
        index++;
    }

    Temp_tempList calleesaves = F_calleesaves();
    Temp_temp saveRbx = Temp_newtemp();
    Temp_temp saveRbp = Temp_newtemp();
    Temp_temp saveR12 = Temp_newtemp();
    Temp_temp saveR13 = Temp_newtemp();
    Temp_temp saveR14 = Temp_newtemp();
    Temp_temp saveR15 = Temp_newtemp();

    index = 1;
    while (index <= 6)
    {
        switch (index)
        {
        case 1:
            emit(AS_Move("movq `s0, `d0", L(saveRbx, NULL), L(F_calleesavesIndex(index), NULL)));
            break;
        case 2:
            break;
        case 3:
            emit(AS_Move("movq `s0, `d0", L(saveR12, NULL), L(F_calleesavesIndex(index), NULL)));
            break;
        case 4:
            emit(AS_Move("movq `s0, `d0", L(saveR13, NULL), L(F_calleesavesIndex(index), NULL)));
            break;
        case 5:
            emit(AS_Move("movq `s0, `d0", L(saveR14, NULL), L(F_calleesavesIndex(index), NULL)));
            break;
        case 6:
            emit(AS_Move("movq `s0, `d0", L(saveR15, NULL), L(F_calleesavesIndex(index), NULL)));
            break;
        }
        index++;
    }
    while (stmList)
    {
        munchStm(stmList->head);
        stmList = stmList->tail;
    }
    index = 1;
    while (index <= 6)
    {
        switch (index)
        {
        case 1:
            emit(AS_Move("movq `s0, `d0", L(F_calleesavesIndex(index), NULL), L(saveRbx, NULL)));
            break;
        case 2:
            break;
        case 3:
            emit(AS_Move("movq `s0, `d0", L(F_calleesavesIndex(index), NULL), L(saveR12, NULL)));
            break;
        case 4:
            emit(AS_Move("movq `s0, `d0", L(F_calleesavesIndex(index), NULL), L(saveR13, NULL)));
            break;
        case 5:
            emit(AS_Move("movq `s0, `d0", L(F_calleesavesIndex(index), NULL), L(saveR14, NULL)));
            break;
        case 6:
            emit(AS_Move("movq `s0, `d0", L(F_calleesavesIndex(index), NULL), L(saveR15, NULL)));
            break;
        }
        index++;
    }
    char endFrame[INSTLENGTH];
    sprintf(endFrame, "addq $%s, `d0", String(rbpConvert));
    emit(AS_Oper(String(endFrame), Temp_TempList(F_RSP(), NULL), Temp_TempList(F_RSP(), NULL), NULL));
    emit(AS_Oper("ret", NULL, F_calleesaves(), NULL));
    return iList;
}
