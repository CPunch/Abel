#include "entity.h"

#include "core/mem.h"
#include "core/vec2.h"
#include "game.h"
#include "sprite.h"
#include "map.h"
#include "layer.h"

/* BOILERPLATE !!!! */

tAbelE_entity *AbelE_newEntity(tAbel_fVec2 pos)
{
    tAbelE_entity *entity = (tAbelE_entity *)AbelM_malloc(sizeof(tAbelE_entity));

    entity->velocity = AbelV_newfVec2(0, 0);
    entity->collider = AbelV_newiVec2(TILESET_SIZE, TILESET_SIZE);
    entity->sprite = AbelS_newSprite(AbelG_getLayer(LAYER_ENTITY), pos);
    entity->id = AbelM_addEntity(entity);
    return entity;
}

void AbelE_freeEntity(tAbelE_entity *entity)
{
    AbelM_rmvEntity(entity->id);
    AbelS_freeSprite(entity->sprite);
    AbelM_free(entity);
}

void AbelE_setPosition(tAbelE_entity *entity, tAbel_fVec2 pos)
{
    AbelS_setSpritePos(entity->sprite, pos);
}

void AbelE_setVelocity(tAbelE_entity *entity, tAbel_fVec2 velo)
{
    entity->velocity = velo;
}

tAbel_fVec2 AbelE_getPosition(tAbelE_entity *entity)
{
    return entity->sprite->pos;
}

tAbel_fVec2 AbelE_getVelocity(tAbelE_entity *entity)
{
    return entity->velocity;
}

tAbel_iVec2 AbelE_getCollider(tAbelE_entity *entity)
{
    return entity->collider;
}

void AbelE_stepEntity(tAbelE_entity *entity, uint32_t delta)
{
    tAbel_fVec2 deltaTime = AbelV_newfVec2(delta / 1000.0, delta / 1000.0);
    tAbel_fVec2 deltaPos = AbelV_mulfVec2(entity->velocity, deltaTime);
    tAbel_fVec2 newPos = AbelV_addfVec2(entity->sprite->pos, deltaPos);
    tAbelE_entity *other;

}