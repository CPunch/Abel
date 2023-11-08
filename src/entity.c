#include "entity.h"

#include "assets.h"
#include "chunk.h"
#include "core/mem.h"
#include "core/vec2.h"
#include "game.h"
#include "render.h"
#include "sprite.h"
#include "world.h"

void AbelE_initEntity(tAbelE_entity *entity, tAbelV_fVec2 pos)
{
    tAbelR_texture *tileSet = AbelA_getTexture(ASSET_ID_ENTITY_TILESET);

    entity->velocity = AbelV_newfVec2(0, 0);
    entity->collider = AbelV_newiVec2(TILESET_SIZE - 1, TILESET_SIZE - 1);
    entity->id = AbelW_addEntity(entity);

    AbelS_initSprite(&entity->sprite, tileSet, pos);
}

void AbelE_cleanupEntity(tAbelE_entity *entity)
{
    AbelW_rmvEntity(entity->id);
    AbelS_cleanupSprite(&entity->sprite);
}

void AbelE_setPosition(tAbelE_entity *entity, tAbelV_fVec2 pos)
{
    AbelS_setSpritePos(&entity->sprite, pos);
}

void AbelE_setVelocity(tAbelE_entity *entity, tAbelV_fVec2 velo)
{
    entity->velocity = velo;
}

tAbelV_fVec2 AbelE_getPosition(tAbelE_entity *entity)
{
    return entity->sprite.pos;
}

tAbelV_fVec2 AbelE_getVelocity(tAbelE_entity *entity)
{
    return entity->velocity;
}

tAbelV_iVec2 AbelE_getCollider(tAbelE_entity *entity)
{
    return entity->collider;
}

static bool checkCell(tAbelV_iVec2 worldPos)
{
    return AbelW_getCell(AbelC_posToGrid(worldPos)).isSolid;
}

void AbelE_stepEntity(tAbelE_entity *entity, uint32_t delta)
{
    tAbelV_fVec2 deltaTime = AbelV_newfVec2(delta / 1000.0, delta / 1000.0);
    tAbelV_fVec2 deltaPos = AbelV_mulfVec2(entity->velocity, deltaTime);
    tAbelV_fVec2 oldPos = entity->sprite.pos;
    tAbelV_fVec2 newPos = AbelV_addfVec2(oldPos, deltaPos);

    /* check left/right colliders */
    if (deltaPos.x > 0) {
        if (checkCell(AbelV_newiVec2((int)newPos.x + entity->collider.x, (int)oldPos.y)) ||
            checkCell(AbelV_newiVec2((int)newPos.x + entity->collider.x, (int)oldPos.y + entity->collider.y))) {
            newPos.x = oldPos.x;
        }
    } else if (deltaPos.x < 0) {
        if (checkCell(AbelV_newiVec2((int)newPos.x, (int)oldPos.y)) || checkCell(AbelV_newiVec2((int)newPos.x, (int)oldPos.y + entity->collider.y))) {
            newPos.x = oldPos.x;
        }
    }

    /* check up/down colliders */
    if (deltaPos.y > 0) {
        if (checkCell(AbelV_newiVec2((int)oldPos.x + entity->collider.x, (int)newPos.y + entity->collider.y)) ||
            checkCell(AbelV_newiVec2((int)oldPos.x, (int)newPos.y + entity->collider.y))) {
            newPos.y = oldPos.y;
        }
    } else if (deltaPos.y < 0) {
        if (checkCell(AbelV_newiVec2((int)oldPos.x, (int)newPos.y)) || checkCell(AbelV_newiVec2((int)oldPos.x + entity->collider.x, (int)newPos.y))) {
            newPos.y = oldPos.y;
        }
    }

    /* update position */
    AbelE_setPosition(entity, newPos);
}

void AbelE_renderEntity(tAbelE_entity *entity)
{
    AbelS_drawSprite(&entity->sprite);
}