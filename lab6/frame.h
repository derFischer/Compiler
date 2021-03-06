
/*Lab5: This header file is not complete. Please finish it with more definition.*/

#ifndef FRAME_H
#define FRAME_H

#include "tree.h"
#define WORDSIZE 8

typedef struct F_frame_ *F_frame;

typedef struct F_access_ *F_access;
typedef struct F_accessList_ *F_accessList;

struct F_accessList_ {F_access head; F_accessList tail;};
struct F_access_
{
	enum
	{
		inFrame,
		inReg
	} kind;
	union {
		int offset;	//inFrame
		Temp_temp reg; //inReg
	} u;
};

/* declaration for fragments */
Temp_map F_tempMap;
typedef struct F_frag_ *F_frag;
struct F_frag_ {enum {F_stringFrag, F_procFrag} kind;
			union {
				struct {Temp_label label; string str;} stringg;
				struct {T_stm body; F_frame frame;} proc;
			} u;
};

F_frag F_StringFrag(Temp_label label, string str);
F_frag F_ProcFrag(T_stm body, F_frame frame);

typedef struct F_fragList_ *F_fragList;
struct F_fragList_ 
{
	F_frag head; 
	F_fragList tail;
};

F_fragList F_FragList(F_frag head, F_fragList tail);

/* declaration for frames */
F_frame F_newFrame(Temp_label name, U_boolList formals);
Temp_label F_name(F_frame f);
F_accessList F_formals(F_frame f);
F_access F_allocLocal(F_frame f, bool escaple);

Temp_temp F_FP(void);
Temp_temp F_RV(void);
Temp_temp F_argsReg(int index);
T_exp F_exp(F_access acc, T_exp framePtr);
T_exp F_externalCall(string s, T_expList args);
T_stm F_procEntryExit1(F_frame frame, T_stm stm);
Temp_tempList F_calleesaves();
Temp_tempList F_callersaves();
Temp_tempList F_allRegisters();
int F_accessOffset(F_access access);
int F_frameLength(F_frame f);
Temp_temp F_calleesavesIndex(int index);

Temp_temp F_RSP(void);
Temp_temp F_RBP(void);
Temp_temp F_RAX(void);
Temp_temp F_RDI(void);
Temp_temp F_RSI(void);
Temp_temp F_RDX(void);
Temp_temp F_RCX(void);
Temp_temp F_R8(void);
Temp_temp F_R9(void);
Temp_temp F_R10(void);
Temp_temp F_R11(void);
Temp_temp F_R12(void);
Temp_temp F_R13(void);
Temp_temp F_R14(void);
Temp_temp F_R15(void);
Temp_temp F_RBX(void);
#endif
