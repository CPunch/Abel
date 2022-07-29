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
    bool collide = false;

    /* colliders */
    tAbel_iVec2 TLCorner = AbelL_posToGrid(AbelV_f2iVec(newPos));
    tAbel_iVec2 TRCorner = AbelL_posToGrid(AbelV_newiVec2((int)newPos.x + entity->collider.x, (int)newPos.y));
    tAbel_iVec2 BLCorner = AbelL_posToGrid(AbelV_newiVec2((int)newPos.x, (int)newPos.y + entity->collider.y));
    tAbel_iVec2 BRCorner = AbelL_posToGrid(AbelV_newiVec2((int)newPos.x + entity->collider.x, (int)newPos.y + entity->collider.y));


    /* check left/right colliders */
    if (deltaPos.x > 0) {
        if (AbelM_getCell(TRCorner).isSolid || AbelM_getCell(BRCorner).isSolid) {
            newPos.x = entity->sprite->pos.x;
            collide = true;
        }
    } else if (deltaPos.x < 0) {
        if (AbelM_getCell(TLCorner).isSolid || AbelM_getCell(BLCorner).isSolid) {
            newPos.x = entity->sprite->pos.x;
            collide = true;
        }
    }

    /* check up/down colliders */
    if (deltaPos.y > 0) {
        if (AbelM_getCell(BRCorner).isSolid || AbelM_getCell(BLCorner).isSolid) {
            newPos.y = entity->sprite->pos.y;
            collide = true;
        }
    } else if (deltaPos.y < 0) {
        if (AbelM_getCell(TLCorner).isSolid || AbelM_getCell(TRCorner).isSolid) {
            newPos.y = entity->sprite->pos.y;
            collide = true;
        }
    }

    /* update position */
    AbelE_setPosition(entity, newPos);
}