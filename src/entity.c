#include "entity.h"
#include "sprite.h"
#include "game.h"

#include "core/mem.h"
#include "core/vec2.h"
#include "world.h"

/* BOILERPLATE !!!! */

tAbelE_entity *AbelE_newEntity(tAbel_fVec2 pos)
{
    tAbelE_entity *entity = (tAbelE_entity *)AbelM_malloc(sizeof(tAbelE_entity));

    entity->sprite = AbelS_newSprite(AbelG_getLayer(LAYER_ENTITY), pos);
    entity->id = AbelW_addEntity(entity);
    return entity;
}

void AbelE_freeEntity(tAbelE_entity *entity)
{
    AbelW_rmvEntity(entity->id);
    AbelM_free(entity);
}

void AbelE_stepEntity(tAbelE_entity *entity, uint32_t delta)
{
    tAbel_fVec2 deltaTime = AbelV_newfVec2(delta/1000.0, delta/1000.0);
    tAbel_fVec2 deltaPos = AbelV_mulfVec2(entity->velocity, deltaTime);
    tAbel_fVec2 newPos = AbelV_addfVec2(entity->sprite->pos, deltaPos);

    /* add delta to position */
    AbelS_setSpritePos(entity->sprite, newPos);
}