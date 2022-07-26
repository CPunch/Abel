#ifndef ABEL_ENTITY_H
#define ABEL_ENTITY_H

#include "abel.h"
#include "sprite.h"

#include "core/vec2.h"

typedef struct _tAbelE_entity
{
    tAbelS_sprite sprite;
    tAbel_fVec2 velocity;
    tAbel_iVec2 collider; /* size of collider */
    ENTITY_ID id;
} tAbelE_entity;

tAbelE_entity *AbelE_newEntity(void);
void AbelE_freeEntity(tAbelE_entity *entity);

#endif