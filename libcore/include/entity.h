#ifndef ABEL_ENTITY_H
#define ABEL_ENTITY_H

#include "abel.h"
#include "core/mem.h"
#include "core/vec2.h"
#include "sprite.h"

typedef struct _tAbelE_entity
{
    tAbelM_refCount refCount;
    tAbelS_sprite sprite;
    tAbelV_fVec2 velocity;
    tAbelV_iVec2 collider; /* size of collider */
    tAbelC_chunk *currentChunk;
    struct _tAbelE_entity *nextRender; /* used to compute render order */
    int luaRef;                        /* used to store a reference to the lua userdata table */
} tAbelE_entity;

void AbelE_defaultFree(tAbelM_refCount *ptr);

/* TODO: maybe accept a collider here as well? */
void AbelE_initEntity(tAbelE_entity *entity, tAbelR_texture *tileSet, tAbelV_fVec2 pos, void (*free)(tAbelM_refCount *ptr));
void AbelE_cleanupEntity(tAbelE_entity *entity);

void AbelE_setPosition(tAbelE_entity *entity, tAbelV_fVec2 pos);
void AbelE_setVelocity(tAbelE_entity *entity, tAbelV_fVec2 velo);
void AbelE_setCollider(tAbelE_entity *entity, tAbelV_iVec2 collider);
void AbelE_setChunk(tAbelE_entity *entity, tAbelC_chunk *chunk);
void AbelE_setRef(tAbelE_entity *entity, int ref);

tAbelV_fVec2 AbelE_getPosition(tAbelE_entity *entity);
tAbelV_fVec2 AbelE_getVelocity(tAbelE_entity *entity);
tAbelV_iVec2 AbelE_getCollider(tAbelE_entity *entity);
tAbelC_chunk *AbelE_getChunk(tAbelE_entity *entity);
int AbelE_getRef(tAbelE_entity *entity);

/* utils */
void AbelE_stepEntity(tAbelE_entity *entity, uint32_t delta);
void AbelE_renderEntity(tAbelE_entity *entity);

#endif