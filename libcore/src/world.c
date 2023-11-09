#include "world.h"

#include "chunk.h"
#include "core/hashmap.h"
#include "core/mem.h"
#include "core/tasks.h"
#include "entity.h"
#include "game.h"
#include "render.h"

typedef struct _tAbelW_state
{
    tAbelT_task *stepTimer;
    struct hashmap *entityMap;
    struct hashmap *chunkMap;
    ENTITY_ID nextEntityID;
    uint32_t lastStepTime;
} tAbelW_state;

static tAbelW_state AbelW_state = {0};

/* ===================================[[ Helper Functions ]]==================================== */

typedef struct
{
    ENTITY_ID id;
    tAbelE_entity *entity;
} tAbelW_entityElem;

typedef struct
{
    tAbelV_iVec2 pos;
    tAbelC_chunk *chunk;
} tAbelW_chunkElem;

/* =========================================[[ Entity ]]======================================== */

static uint64_t entityHash(const void *item, uint64_t seed0, uint64_t seed1)
{
    tAbelW_entityElem *elem = (tAbelW_entityElem *)item;
    return elem->id;
}

static int entityCompare(const void *a, const void *b, void *udata)
{
    tAbelW_entityElem *elem1 = (tAbelW_entityElem *)a;
    tAbelW_entityElem *elem2 = (tAbelW_entityElem *)b;
    return elem1->id != elem2->id;
}

static ENTITY_ID getNextEntityID()
{
    return AbelW_state.nextEntityID++;
}

static ENTITY_ID addEntity(tAbelE_entity *entity)
{
    ENTITY_ID nextID = getNextEntityID();

    /* add to entity map */
    hashmap_set(AbelW_state.entityMap, &(tAbelW_entityElem){.id = nextID, .entity = entity});
    return nextID;
}

static void rmvEntity(ENTITY_ID id)
{
    hashmap_delete(AbelW_state.entityMap, &(tAbelW_entityElem){.id = id});
}

static tAbelE_entity *getEntity(ENTITY_ID id)
{
    tAbelW_entityElem *elem = hashmap_get(AbelW_state.entityMap, &(tAbelW_entityElem){.id = id});
    return elem ? elem->entity : NULL;
}

ENTITY_ID AbelW_addEntity(tAbelE_entity *entity)
{
    return addEntity(entity);
}

void AbelW_rmvEntity(ENTITY_ID id)
{
    return rmvEntity(id);
}

/* check for collision against other entities */
tAbelE_entity *AbelW_checkEntityCollide(tAbelE_entity *entity)
{
    size_t i = 0;
    void *item;
    tAbelE_entity *other;
    tAbelV_fVec2 pos = AbelE_getPosition(entity), otherPos;
    tAbelV_iVec2 collider = AbelE_getCollider(entity), otherCollider;

    /* walk through each entity, check for collision */
    /* TODO: split entities into 'chunks' */
    while (hashmap_iter(AbelW_state.entityMap, &i, &item)) {
        other = ((tAbelW_entityElem *)item)->entity;
        otherPos = AbelE_getPosition(other);
        otherCollider = AbelE_getCollider(other);

        /* check collide */
        if (pos.x < otherPos.x + otherCollider.x && pos.x + collider.x > otherPos.x && pos.y < otherPos.y + otherCollider.y && pos.y + collider.y > otherPos.y)
            return other;
    }

    /* no collisions found! */
    return NULL;
}

tAbelE_entity *AbelW_getEntity(ENTITY_ID id)
{
    return getEntity(id);
}

void AbelW_renderEntities(void)
{
    size_t i = 0;
    void *item;
    tAbelW_entityElem *elem;

    /* render each entity */
    while (hashmap_iter(AbelW_state.entityMap, &i, &item)) {
        elem = (tAbelW_entityElem *)item;
        AbelE_renderEntity(elem->entity);
    }
}

/* =========================================[[ Chunk ]]========================================= */

static uint64_t chunkHash(const void *item, uint64_t seed0, uint64_t seed1)
{
    tAbelW_chunkElem *elem = (tAbelW_chunkElem *)item;
    int hash = 23 + (seed0 ^ seed1);

    /* do not question the magic numbers */
    hash = (hash * 37) + elem->pos.x;
    hash = (hash * 37) + elem->pos.y;
    return hash;
}

static int chunkCompare(const void *a, const void *b, void *udata)
{
    tAbelW_chunkElem *elem1 = (tAbelW_chunkElem *)a;
    tAbelW_chunkElem *elem2 = (tAbelW_chunkElem *)b;
    return AbelV_compareiVec2(elem1->pos, elem2->pos);
}

static uint32_t worldStepTask(uint32_t delta, void *uData)
{
    size_t i = 0;
    void *item;
    tAbelW_entityElem *elem;

    while (hashmap_iter(AbelW_state.entityMap, &i, &item)) {
        elem = (tAbelW_entityElem *)item;
        AbelE_stepEntity(elem->entity, delta);
    }

    AbelW_state.lastStepTime = SDL_GetTicks();
    return WORLD_STEP_INTERVAL;
}

static tAbelC_chunk *addChunk(tAbelV_iVec2 pos)
{
    tAbelC_chunk *chunk = AbelC_newChunk(pos);

    /* add to chunk map */
    hashmap_set(AbelW_state.chunkMap, &(tAbelW_chunkElem){.pos = pos, .chunk = chunk});
    return chunk;
}

static void rmvChunk(tAbelV_iVec2 pos)
{
    hashmap_delete(AbelW_state.chunkMap, &(tAbelW_chunkElem){.pos = pos});
}

static tAbelC_chunk *getChunk(tAbelV_iVec2 pos)
{
    tAbelW_chunkElem *elem = hashmap_get(AbelW_state.chunkMap, &(tAbelW_chunkElem){.pos = pos});
    return elem ? elem->chunk : NULL;
}

tAbelC_chunk *AbelW_getChunk(tAbelV_iVec2 chunkPos)
{
    return getChunk(chunkPos);
}

tAbelV_iVec2 AbelW_getChunkPos(tAbelV_iVec2 cellPos)
{
    return AbelV_diviVec2(cellPos, AbelC_chunkSize);
}

void AbelW_renderChunks(LAYER_ID layer)
{
    size_t i = 0;
    void *item;
    tAbelW_chunkElem *elem;

    /* render each chunk */
    while (hashmap_iter(AbelW_state.chunkMap, &i, &item)) {
        elem = (tAbelW_chunkElem *)item;
        AbelC_renderChunk(elem->chunk, layer);
    }
}

/* =====================================[[ Initializers ]]====================================== */

void AbelW_init(void)
{
    AbelW_state.entityMap = hashmap_new(sizeof(tAbelW_entityElem), 8, 0, 0, entityHash, entityCompare, NULL, NULL);
    AbelW_state.chunkMap = hashmap_new(sizeof(tAbelW_chunkElem), 4, 0, 0, chunkHash, chunkCompare, NULL, NULL);
    AbelW_state.stepTimer = AbelT_newTask(WORLD_STEP_INTERVAL, worldStepTask, NULL);
    AbelW_state.nextEntityID = 0;
    AbelW_state.lastStepTime = SDL_GetTicks();
}

void AbelW_quit(void)
{
    hashmap_free(AbelW_state.entityMap);

    /* free chunks */
    size_t i = 0;
    void *item;
    tAbelW_chunkElem *elem;
    while (hashmap_iter(AbelW_state.chunkMap, &i, &item)) {
        elem = (tAbelW_chunkElem *)item;
        AbelC_freeChunk(elem->chunk);
    }

    hashmap_free(AbelW_state.chunkMap);

    AbelT_freeTask(AbelW_state.stepTimer);
}

/* =========================================[[ Cells ]]========================================= */

void AbelW_setCell(tAbelV_iVec2 pos, TILE_ID id, bool isSolid)
{
    /* update chunk */
    tAbelV_iVec2 chunkPos = AbelW_getChunkPos(pos), localPos;
    tAbelC_chunk *chunk = AbelW_getChunk(chunkPos);

    /* if chunk doesn't exist, create and add it to chunk map */
    if (chunk == NULL)
        chunk = addChunk(chunkPos);

    localPos = AbelC_worldPosToLocalPos(chunk, pos);
    AbelC_drawTile(chunk, localPos, id, LAYER_BG);
    AbelC_setCell(chunk, localPos, id, isSolid);
}

tAbelW_cell AbelW_getCell(tAbelV_iVec2 pos)
{
    tAbelC_chunk *chunk = AbelW_getChunk(AbelW_getChunkPos(pos));

    /* if chunk doesn't exist, return default cell */
    if (chunk == NULL)
        return (tAbelW_cell){.id = 0, .isSolid = true};

    tAbelV_iVec2 localPos = AbelC_worldPosToLocalPos(chunk, pos);
    return AbelC_getCell(chunk, localPos);
}