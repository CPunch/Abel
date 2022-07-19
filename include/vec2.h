#ifndef ABEL_VEC2_H
#define ABEL_VEC2_H

#include "abel.h"

typedef struct
{
    uint32_t x;
    uint32_t y;
} tAbel_vec2;

#define AbelV_newVec2(_x, _y)                                                                      \
    (tAbel_vec2)                                                                                   \
    {                                                                                              \
        .x = _x, .y = _y                                                                           \
    }
#define AbelV_addVec2(v1, v2) (AbelV_newVec2(v1.x + v2.x, v1.y + v2.y))
#define AbelV_subVec2(v1, v2) (AbelV_newVec2(v1.x - v2.x, v1.y - v2.y))
#define AbelV_mulVec2(v1, v2) (AbelV_newVec2(v1.x * v2.x, v1.y * v2.y))
#define AbelV_divVec2(v1, v2) (AbelV_newVec2(v1.x / v2.x, v1.y / v2.y))

#endif