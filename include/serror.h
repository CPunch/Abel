#ifndef ABEL_ERROR_H
#define ABEL_ERROR_H

#include "abel.h"

#include <setjmp.h>
#include <stdio.h>

/* defines errorstack size */
#define ABEL_MAXERRORS 32

/* DO NOT RETURN/GOTO/BREAK or otherwise skip ABEL_TRYEND */
#define ABEL_TRY       if (setjmp(eAbel_errStack[++eAbel_errIndx]) == 0) {
#define ABEL_CATCH                                                                                 \
    }                                                                                              \
    else                                                                                           \
    {
#define ABEL_TRYEND                                                                                \
    }                                                                                              \
    --eAbel_errIndx;

/* if eAbel_errIndx is >= 0, we have a safe spot to jump too if an error is thrown */
#define ABEL_ISPROTECTED (eAbel_errIndx >= 0)

/* ABEL_ERROR(printf args):
        if called after a ABEL_TRY block will jump to the previous ABEL_CATCH/ABEL_TRYEND block,
    otherwise program is exit()'d. if ABEL_QUITE is defined printf is called with passed args, else
    arguments are ignored.
*/
#ifndef ABEL_QUITE
#    define ABEL_ERROR(...)                                                                        \
        do {                                                                                       \
            if (ABEL_ISPROTECTED)                                                                  \
                longjmp(eAbel_errStack[eAbel_errIndx], 1);                                         \
            else                                                                                   \
                exit(1);                                                                           \
        } while (0);
#    define ABEL_WARN(...) ((void)0) /* no op */
#else
#    define ABEL_ERROR(...)                                                                        \
        do {                                                                                       \
            printf("[ERROR] : " __VA_ARGS__);                                                      \
            if (ABEL_ISPROTECTED)                                                                  \
                longjmp(eAbel_errStack[eAbel_errIndx], 1);                                         \
            else                                                                                   \
                exit(1);                                                                           \
        } while (0);

#    define ABEL_WARN(...) printf("[WARN] : " __VA_ARGS__);
#endif

extern int eAbel_errIndx;
extern jmp_buf eAbel_errStack[ABEL_MAXERRORS];

#endif