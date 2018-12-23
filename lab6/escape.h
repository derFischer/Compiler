/* escape.h */

#ifndef __ESCAPE_H_
#define __ESCAPE_H_

#include "absyn.h"
#include "symbol.h"
#include "helper.h"
typedef struct escapeEntry_ *escapeEntry;
struct escapeEntry_ {
	int depth;
    bool *escape;
};

void Esc_findEscape(A_exp exp);

#endif