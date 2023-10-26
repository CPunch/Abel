#ifndef ABEL_VEC2_H
#define ABEL_VEC2_H

#include "abel.h"

typedef struct
{
    uint32_t x;
    uint32_t y;
} tAbelV_iVec2;

typedef struct
{
    double x;
    double y;
} tAbelV_fVec2;

/* ======================================[[ Int Vector ]]======================================= */

#define AbelV_newiVec2(_x, _y)                                                                                                                                 \
    (tAbelV_iVec2)                                                                                                                                              \
    {                                                                                                                                                          \
        .x = _x, .y = _y                                                                                                                                       \
    }

// tAbelV_iVec2 AbelV_newiVec2(uint32_t _x, uint32_t _y)
// {
//     return (tAbelV_iVec2){
//         .x = _x, .y = _y
//     };
// }

static inline tAbelV_iVec2 AbelV_addiVec2(tAbelV_iVec2 v1, tAbelV_iVec2 v2)
{
    return AbelV_newiVec2(v1.x + v2.x, v1.y + v2.y);
}

static inline tAbelV_iVec2 AbelV_subiVec2(tAbelV_iVec2 v1, tAbelV_iVec2 v2)
{
    return AbelV_newiVec2(v1.x - v2.x, v1.y - v2.y);
}

static inline tAbelV_iVec2 AbelV_muliVec2(tAbelV_iVec2 v1, tAbelV_iVec2 v2)
{
    return AbelV_newiVec2(v1.x * v2.x, v1.y * v2.y);
}

static inline tAbelV_iVec2 AbelV_diviVec2(tAbelV_iVec2 v1, tAbelV_iVec2 v2)
{
    return AbelV_newiVec2(v1.x / v2.x, v1.y / v2.y);
}

static inline tAbelV_iVec2 AbelV_f2iVec(tAbelV_fVec2 v)
{
    return AbelV_newiVec2((int)v.x, (int)v.y);
}

// returns true if v1 and v2 differ
static inline bool AbelV_compareiVec2(tAbelV_iVec2 v1, tAbelV_iVec2 v2)
{
    return v1.x != v2.x || v1.y != v2.y;
}

/* =====================================[[ Float Vector ]]====================================== */

#define AbelV_newfVec2(_x, _y)                                                                                                                                 \
    (tAbelV_fVec2)                                                                                                                                              \
    {                                                                                                                                                          \
        .x = _x, .y = _y                                                                                                                                       \
    }

// tAbelV_fVec2 AbelV_newfVec2(double _x, double _y)
// {
//     return (tAbelV_fVec2){
//         .x = _x, .y = _y
//     };
// }

static inline tAbelV_fVec2 AbelV_addfVec2(tAbelV_fVec2 v1, tAbelV_fVec2 v2)
{
    return AbelV_newfVec2(v1.x + v2.x, v1.y + v2.y);
}

static inline tAbelV_fVec2 AbelV_subfVec2(tAbelV_fVec2 v1, tAbelV_fVec2 v2)
{
    return AbelV_newfVec2(v1.x - v2.x, v1.y - v2.y);
}

static inline tAbelV_fVec2 AbelV_mulfVec2(tAbelV_fVec2 v1, tAbelV_fVec2 v2)
{
    return AbelV_newfVec2(v1.x * v2.x, v1.y * v2.y);
}

static inline tAbelV_fVec2 AbelV_divfVec2(tAbelV_fVec2 v1, tAbelV_fVec2 v2)
{
    return AbelV_newfVec2(v1.x / v2.x, v1.y / v2.y);
}

static inline tAbelV_fVec2 AbelV_i2fVec(tAbelV_iVec2 v)
{
    return AbelV_newfVec2((double)v.x, (double)v.y);
}

#endif