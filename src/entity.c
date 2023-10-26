#include "entity.h"

#include "core/mem.h"
#include "core/vec2.h"
#include "game.h"
#include "map.h"
#include "sprite.h"
#include "render.h"
#include "chunk.h"

tAbelE_entity *AbelE_newEntity(tAbelR_texture *tileSet, tAbelV_fVec2 pos)
{
    tAbelE_entity *entity = (tAbelE_entity *)AbelM_malloc(sizeof(tAbelE_entity));

    entity->velocity = AbelV_newfVec2(0, 0);
    entity->collider = AbelV_newiVec2(TILESET_SIZE - 1, TILESET_SIZE - 1);
    entity->sprite = AbelS_newSprite(tileSet, pos);
    entity->id = AbelM_addEntity(entity);
    return entity;
}

void AbelE_freeEntity(tAbelE_entity *entity)
{
    AbelM_rmvEntity(entity->id);
    AbelS_freeSprite(entity->sprite);
    AbelM_free(entity);
}

void AbelE_setPosition(tAbelE_entity *entity, tAbelV_fVec2 pos)
{
    AbelS_setSpritePos(entity->sprite, pos);
}

void AbelE_setVelocity(tAbelE_entity *entity, tAbelV_fVec2 velo)
{
    entity->velocity = velo;
}

tAbelV_fVec2 AbelE_getPosition(tAbelE_entity *entity)
{
    return entity->sprite->pos;
}

tAbelV_fVec2 AbelE_getVelocity(tAbelE_entity *entity)
{
    return entity->velocity;
}

tAbelV_iVec2 AbelE_getCollider(tAbelE_entity *entity)
{
    return entity->collider;
}

void AbelE_stepEntity(tAbelE_entity *entity, uint32_t delta)
{
    tAbelV_fVec2 deltaTime = AbelV_newfVec2(delta / 1000.0, delta / 1000.0);
    tAbelV_fVec2 deltaPos = AbelV_mulfVec2(entity->velocity, deltaTime);
    tAbelV_fVec2 oldPos = entity->sprite->pos;
    tAbelV_fVec2 newPos = AbelV_addfVec2(oldPos, deltaPos);

    /* check left/right colliders */
    if (deltaPos.x > 0) {
        if (AbelM_getCell(AbelC_posToGrid(AbelV_newiVec2((int)newPos.x + entity->collider.x, (int)oldPos.y))).isSolid ||
            AbelM_getCell(AbelC_posToGrid(AbelV_newiVec2((int)newPos.x + entity->collider.x, (int)oldPos.y + entity->collider.y))).isSolid) {
            newPos.x = oldPos.x;
        }
    } else if (deltaPos.x < 0) {
        if (AbelM_getCell(AbelC_posToGrid(AbelV_newiVec2((int)newPos.x, (int)oldPos.y))).isSolid ||
            AbelM_getCell(AbelC_posToGrid(AbelV_newiVec2((int)newPos.x, (int)oldPos.y + entity->collider.y))).isSolid) {
            newPos.x = oldPos.x;
        }
    }

    /* check up/down colliders */
    if (deltaPos.y > 0) {
        if (AbelM_getCell(AbelC_posToGrid(AbelV_newiVec2((int)oldPos.x + entity->collider.x, (int)newPos.y + entity->collider.y))).isSolid ||
            AbelM_getCell(AbelC_posToGrid(AbelV_newiVec2((int)oldPos.x, (int)newPos.y + entity->collider.y))).isSolid) {
            newPos.y = oldPos.y;
        }
    } else if (deltaPos.y < 0) {
        if (AbelM_getCell(AbelC_posToGrid(AbelV_newiVec2((int)oldPos.x, (int)newPos.y))).isSolid ||
            AbelM_getCell(AbelC_posToGrid(AbelV_newiVec2((int)oldPos.x + entity->collider.x, (int)newPos.y))).isSolid) {
            newPos.y = oldPos.y;
        }
    }

    /* update position */
    AbelE_setPosition(entity, newPos);
}

void AbelE_renderEntity(tAbelE_entity *entity)
{
    AbelS_drawSprite(entity->sprite);
}