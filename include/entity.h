#ifndef ABEL_ENTITY_H
#define ABEL_ENTITY_H

#include "abel.h"
#include "core/vec2.h"

typedef struct _tAbelE_entity
{
    tAbelS_sprite *sprite;
    tAbel_fVec2 velocity;
    tAbel_iVec2 collider; /* size of collider */
    ENTITY_ID id;
} tAbelE_entity;

tAbelE_entity *AbelE_newEntity(tAbel_fVec2 pos);
void AbelE_freeEntity(tAbelE_entity *entity);

void AbelE_setVelocity(tAbelE_entity *entity, tAbel_fVec2 velo);

/* utils */
void AbelE_stepEntity(tAbelE_entity *entity, uint32_t delta);

#endif