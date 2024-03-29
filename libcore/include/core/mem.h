#ifndef ABEL_MEM_H
#define ABEL_MEM_H

#include "abel.h"

#define GROW_FACTOR               2

#define AbelM_malloc(sz)          AbelM_realloc(NULL, sz)
#define AbelM_calloc(type, count) (type *)AbelM_realloc(NULL, sizeof(type))
#define AbelM_free(buf)           AbelM_realloc(buf, 0)

/* ========================================[[ Vectors ]]======================================== */

#define AbelM_countVector(name)   name##_COUNT
#define AbelM_capVector(name)     name##_CAP

#define AbelM_newVector(type, name)                                                                                                                            \
    type *name;                                                                                                                                                \
    size_t name##_COUNT;                                                                                                                                       \
    size_t name##_CAP

#define AbelM_initVector(name, startCap)                                                                                                                       \
    name = NULL;                                                                                                                                               \
    name##_COUNT = 0;                                                                                                                                          \
    name##_CAP = startCap

#define AbelM_growVector(type, name, needed)                                                                                                                   \
    if (name##_COUNT + needed >= name##_CAP || name == NULL) {                                                                                                 \
        name##_CAP = (name##_CAP + needed) * GROW_FACTOR;                                                                                                      \
        name = (type *)AbelM_realloc(name, sizeof(type) * name##_CAP);                                                                                         \
    }

/* moves vector elements above indx down by numElem, removing numElem elements at indx */
#define AbelM_rmvVector(name, indx, numElem)                                                                                                                   \
    do {                                                                                                                                                       \
        int _i, _sz = ((name##_COUNT - indx) - numElem);                                                                                                       \
        for (_i = 0; _i < _sz; _i++)                                                                                                                           \
            name[indx + _i] = name[indx + numElem + _i];                                                                                                       \
        name##_COUNT -= numElem;                                                                                                                               \
    } while (0);

/* moves vector elements above indx up by numElem, inserting numElem elements at indx */
#define AbelM_insertVector(name, indx, numElem)                                                                                                                \
    do {                                                                                                                                                       \
        int _i;                                                                                                                                                \
        for (_i = name##_COUNT; _i > indx; _i--)                                                                                                               \
            name[_i] = name[_i - 1];                                                                                                                           \
        name##_COUNT += numElem;                                                                                                                               \
    } while (0);

/* pushes val onto the end of the vector */
#define AbelM_pushVector(type, name, val)                                                                                                                      \
    do {                                                                                                                                                       \
        AbelM_growVector(type, name, 1);                                                                                                                       \
        name[name##_COUNT++] = val;                                                                                                                            \
    } while (0);

/* =======================================[[ RefCount ]]======================================== */

/* these are expected to be the *first* member of a reference counted struct */
typedef struct _tAbelM_refCount
{
    void (*free)(struct _tAbelM_refCount *ptr);
    int refCount;
} tAbelM_refCount;

static inline void AbelM_initRef(tAbelM_refCount *ref, void (*free)(tAbelM_refCount *ptr))
{
    ref->refCount = 1;
    ref->free = free;
}

static inline void AbelM_retainRef(tAbelM_refCount *ref)
{
    ref->refCount++;
}

static inline void AbelM_releaseRef(tAbelM_refCount *ref)
{
    if (--ref->refCount == 0) {
        ref->free(ref);
    }
}

void *AbelM_realloc(void *buf, size_t sz);
char *AbelM_strdup(const char *str);
uint32_t AbelM_fastHash(uint8_t *data, size_t sz);

#endif