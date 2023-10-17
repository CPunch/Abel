#ifndef ABEL_VEC2_H
#define ABEL_VEC2_H

#include "abel.h"

typedef struct
{
    uint32_t x;
    uint32_t y;
} tAbel_iVec2;

typedef struct
{
    double x;
    double y;
} tAbel_fVec2;

/* ======================================[[ Int Vector ]]======================================= */

#define AbelV_newiVec2(_x, _y)                                                                                                                                 \
    (tAbel_iVec2)                                                                                                                                              \
    {                                                                                                                                                          \
        .x = _x, .y = _y                                                                                                                                       \
    }

#define AbelV_addiVec2(v1, v2) (AbelV_newiVec2(v1.x + v2.x, v1.y + v2.y))
#define AbelV_subiVec2(v1, v2) (AbelV_newiVec2(v1.x - v2.x, v1.y - v2.y))
#define AbelV_muliVec2(v1, v2) (AbelV_newiVec2(v1.x * v2.x, v1.y * v2.y))
#define AbelV_diviVec2(v1, v2) (AbelV_newiVec2(v1.x / v2.x, v1.y / v2.y))

/* =====================================[[ Float Vector ]]====================================== */

#define AbelV_newfVec2(_x, _y)                                                                                                                                 \
    (tAbel_fVec2)                                                                                                                                              \
    {                                                                                                                                                          \
        .x = _x, .y = _y                                                                                                                                       \
    }

#define AbelV_addfVec2(v1, v2) (AbelV_newfVec2(v1.x + v2.x, v1.y + v2.y))
#define AbelV_subfVec2(v1, v2) (AbelV_newfVec2(v1.x - v2.x, v1.y - v2.y))
#define AbelV_mulfVec2(v1, v2) (AbelV_newfVec2(v1.x * v2.x, v1.y * v2.y))
#define AbelV_divfVec2(v1, v2) (AbelV_newfVec2(v1.x / v2.x, v1.y / v2.y))

#define AbelV_f2iVec(v)        AbelV_newiVec2((int)v.x, (int)v.y)
#define AbelV_i2fVec(v)        AbelV_newfVec2((double)v.x, (double)v.y)

#endif