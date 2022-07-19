#include "serror.h"

int eAbel_errIndx;
jmp_buf eAbel_errStack[ABEL_MAXERRORS];