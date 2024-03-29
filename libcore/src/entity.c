#include "entity.h"

#include "assets.h"
#include "chunk.h"
#include "core/mem.h"
#include "core/vec2.h"
#include "game.h"
#include "render.h"
#include "script.h"
#include "sprite.h"
#include "world.h"

void AbelE_defaultFree(tAbelM_refCount *ptr)
{
    tAbelE_entity *entity = (tAbelE_entity *)ptr;
    AbelE_cleanupEntity(entity);
    AbelM_free(entity);
}

void AbelE_initEntity(tAbelE_entity *entity, tAbelR_texture *tileSet, tAbelV_fVec2 pos, void (*free)(tAbelM_refCount *ptr))
{
    entity->velocity = AbelV_newfVec2(0, 0);
    entity->collider = AbelV_newiVec2(tileSet->tileSize.x - 1, tileSet->tileSize.y - 1);
    entity->currentChunk = NULL;
    entity->nextRender = NULL;
    entity->luaRef = LUA_NOREF;

    AbelS_initSprite(&entity->sprite, tileSet, pos);
    AbelM_initRef(&entity->refCount, free);
}

void AbelE_cleanupEntity(tAbelE_entity *entity)
{
    AbelS_cleanupSprite(&entity->sprite);

    /* clear userdata from registry */
    lua_State *L = AbelL_globalState();
    if (L && entity->luaRef != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, entity->luaRef);
    }
}

/* ========================================[[ Setters ]]======================================== */

void AbelE_setPosition(tAbelE_entity *entity, tAbelV_fVec2 pos)
{
    AbelS_setSpritePos(&entity->sprite, pos);

    /* if we've been added to the world, make sure to update our current chunk */
    if (entity->currentChunk != NULL) {
        tAbelV_iVec2 gridPos = AbelW_posToGrid(AbelV_f2iVec(pos));
        tAbelV_iVec2 chunkPos = AbelW_getChunkPos(gridPos);
        tAbelC_chunk *chunk = AbelW_getChunk(chunkPos);

        AbelE_setChunk(entity, chunk);
    }
}

void AbelE_setVelocity(tAbelE_entity *entity, tAbelV_fVec2 velo)
{
    entity->velocity = velo;
}

void AbelE_setCollider(tAbelE_entity *entity, tAbelV_iVec2 collider)
{
    entity->collider = collider;
}

void AbelE_setChunk(tAbelE_entity *entity, tAbelC_chunk *chunk)
{
    /* if the entity is already in the chunk, do nothing */
    if (chunk == entity->currentChunk)
        return;

    if (entity->currentChunk != NULL)
        AbelC_rmvEntity(entity->currentChunk, entity);

    if (chunk != NULL)
        AbelC_addEntity(chunk, entity);

    entity->currentChunk = chunk;
}

void AbelE_setRef(tAbelE_entity *entity, int ref)
{
    entity->luaRef = ref;
}

/* ========================================[[ Getters ]]======================================== */

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

tAbelC_chunk *AbelE_getChunk(tAbelE_entity *entity)
{
    return entity->currentChunk;
}

int AbelE_getRef(tAbelE_entity *entity)
{
    return entity->luaRef;
}

static bool checkCell(tAbelV_iVec2 worldPos)
{
    return AbelW_getCell(AbelW_posToGrid(worldPos)).isSolid;
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