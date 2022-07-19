#ifndef ABEL_MEM_H
#define ABEL_MEM_H

#include "abel.h"

#define GROW_FACTOR 2

/* microsoft strikes again with their lack of support for VLAs */
#if _MSC_VER
#    define VLA(type, var, sz) type *var = AbelM_malloc(sizeof(type) * sz);
#    define ENDVLA(var)        AbelM_free(var);
#else
#    define VLA(type, var, sz) type var[sz];
#    define ENDVLA(var)        ((void)0) /* no op */
#endif

#define AbelM_malloc(sz) AbelM_realloc(NULL, sz)
#define AbelM_free(buf)  AbelM_realloc(buf, 0)

#define AbelM_newVector(type, name)                                                                \
    type *name;                                                                                    \
    int name##_COUNT;                                                                              \
    int name##_CAP;
#define AbelM_initVector(name, startCap)                                                           \
    name = NULL;                                                                                   \
    name##_COUNT = 0;                                                                              \
    name##_CAP = startCap;

#define AbelM_growVector(type, name, needed)                                                       \
    if (name##_COUNT + needed >= name##_CAP || buf == NULL) {                                      \
        name##_CAP = (name##_CAP + needed) * GROW_FACTOR;                                          \
        buf = (type *)AbelM_realloc(buf, sizeof(type) * name##_CAP);                               \
    }

/* moves vector elements above indx down by numElem, removing numElem elements at indx */
#define AbelM_rmvVector(name, indx, numElem)                                                       \
    do {                                                                                           \
        int _i, _sz = ((name##_COUNT - indx) - numElem);                                           \
        for (_i = 0; _i < _sz; _i++)                                                               \
            buf[indx + _i] = buf[indx + numElem + _i];                                             \
        name##_COUNT -= numElem;                                                                   \
    } while (0);

/* moves vector elements above indx up by numElem, inserting numElem elements at indx */
#define AbelM_insertVector(buf, indx, numElem)                                                     \
    do {                                                                                           \
        int _i;                                                                                    \
        for (_i = name##_COUNT; _i > indx; _i--)                                                   \
            buf[_i] = buf[_i - 1];                                                                 \
        name##_COUNT += numElem;                                                                   \
    } while (0);

void *AbelM_realloc(void *buf, size_t sz);
uint32_t AbelM_fastHash(uint8_t *data, size_t sz);

#endif