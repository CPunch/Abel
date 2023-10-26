#ifndef ABEL_ENTITY_H
#define ABEL_ENTITY_H

#include "abel.h"
#include "core/vec2.h"

typedef struct _tAbelE_entity
{
    tAbelS_sprite *sprite;
    tAbelV_fVec2 velocity;
    tAbelV_iVec2 collider; /* size of collider */
    ENTITY_ID id;
} tAbelE_entity;

tAbelE_entity *AbelE_newEntity(tAbelR_texture *tileSet, tAbelV_fVec2 pos);
void AbelE_freeEntity(tAbelE_entity *entity);

void AbelE_setPosition(tAbelE_entity *entity, tAbelV_fVec2 pos);
void AbelE_setVelocity(tAbelE_entity *entity, tAbelV_fVec2 velo);

tAbelV_fVec2 AbelE_getPosition(tAbelE_entity *entity);
tAbelV_fVec2 AbelE_getVelocity(tAbelE_entity *entity);
tAbelV_iVec2 AbelE_getCollider(tAbelE_entity *entity);

/* utils */
void AbelE_stepEntity(tAbelE_entity *entity, uint32_t delta);
void AbelE_renderEntity(tAbelE_entity *entity);

#endif