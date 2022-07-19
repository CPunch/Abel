#ifndef ABEL_MEM_H
#define ABEL_MEM_H

#include "abel.h"

#define GROW_FACTOR 2

/* microsoft strikes again with their lack of support for VLAs */
#if _MSC_VER
#    define VLA(type, var, sz) type *var = laikaM_malloc(sizeof(type) * sz);
#    define ENDVLA(var)        laikaM_free(var);
#else
#    define VLA(type, var, sz) type var[sz];
#    define ENDVLA(var)        ((void)0) /* no op */
#endif

#define AbelM_malloc(sz) AbelM_realloc(NULL, sz)
#define AbelM_free(buf)  AbelM_realloc(buf, 0)

#define AbelM_growarray(type, buf, needed, count, capacity)                                        \
    if (count + needed >= capacity || buf == NULL) {                                               \
        capacity = (capacity + needed) * GROW_FACTOR;                                              \
        buf = (type *)AbelM_realloc(buf, sizeof(type) * capacity);                                \
    }

/* moves array elements above indx down by numElem, removing numElem elements at indx */
#define AbelM_rmvArray(buf, count, indx, numElem)                                                  \
    do {                                                                                           \
        int _i, _sz = ((count - indx) - numElem);                                                  \
        for (_i = 0; _i < _sz; _i++)                                                               \
            buf[indx + _i] = buf[indx + numElem + _i];                                             \
        count -= numElem;                                                                          \
    } while (0);

/* moves array elements above indx up by numElem, inserting numElem elements at indx */
#define AbelM_insertArray(buf, count, indx, numElem)                                              \
    do {                                                                                           \
        int _i;                                                                                    \
        for (_i = count; _i > indx; _i--)                                                          \
            buf[_i] = buf[_i - 1];                                                                 \
        count += numElem;                                                                          \
    } while (0);

void *AbelM_realloc(void *buf, size_t sz);
uint32_t AbelM_fastHash(uint8_t *data, size_t sz);

#endif