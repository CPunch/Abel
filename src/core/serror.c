#include "core/serror.h"

int eAbel_errIndx = -1;
jmp_buf eAbel_errStack[ABEL_MAXERRORS];