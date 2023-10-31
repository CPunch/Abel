#include "core/mem.h"

#include "abel.h"
#include "core/serror.h"

/* ======================================[[ Memory API ]]======================================= */

void *AbelM_realloc(void *buf, size_t sz)
{
    void *newBuf;

    /* are we free'ing the buffer? */
    if (sz == 0) {
        free(buf);
        return NULL;
    }

    /* if NULL is passed, realloc() acts like malloc() */
    if ((newBuf = realloc(buf, sz)) == NULL)
        ABEL_ERROR("failed to allocate memory!\n");

    return newBuf;
}

/* NOTE: str *MUST* be a NULL terminated string */
char *AbelM_strdup(const char *str)
{
    char *newStr;
    size_t len;

    len = strlen(str) + 1;
    newStr = (char *)AbelM_malloc(len);

    memcpy(newStr, str, len);
    return newStr;
}

/* tiny implementation of the 'SuperFastHash' algorithm */
uint32_t AbelM_fastHash(uint8_t *data, size_t sz)
{
    size_t i;
    int remainder;
    uint32_t hash = 0, tmp = 0;

    remainder = (sz & 3);
    sz >>= 2;

    for (i = sz; i > 0; i--) {
        hash += *(const uint16_t *)data;
        tmp = (*(const uint16_t *)(data + 2) << 11) ^ hash;
        hash = (hash << 16) ^ tmp;
        data += (2 * sizeof(uint16_t));
        hash += hash >> 11;
    }

    switch (remainder) {
    case 1:
        hash += *data;
        hash ^= hash << 10;
        hash += hash >> 1;
        break;
    case 2:
        hash += *(const uint16_t *)data;
        hash ^= hash << 11;
        hash += hash >> 17;
        break;
    case 3:
        hash += *(const uint16_t *)data;
        hash ^= hash << 16;
        hash ^= data[sizeof(uint16_t)] << 18;
        hash += hash >> 11;
        break;
    }

    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}