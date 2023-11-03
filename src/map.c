#include "map.h"

#include "chunk.h"
#include "core/hashmap.h"
#include "core/mem.h"
#include "core/tasks.h"
#include "entity.h"
#include "game.h"
#include "render.h"

typedef struct _tAbelM_state
{
    tAbelR_texture *tileSet;
    tAbelT_task *stepTimer;
    tAbelT_task *resetFPSTask;
    struct hashmap *entityMap;
    struct hashmap *chunkMap;
    ENTITY_ID nextEntityID;
    uint32_t FPS;
    uint32_t currFPS;
    uint32_t lastStepTime;
} tAbelM_state;

static tAbelM_state AbelM_state = {0};

/* ===================================[[ Helper Functions ]]==================================== */

typedef struct
{
    ENTITY_ID id;
    tAbelE_entity *entity;
} tAbelM_entityElem;

typedef struct
{
    tAbelV_iVec2 pos;
    tAbelC_chunk *chunk;
} tAbelM_chunkElem;

/* =========================================[[ Entity ]]======================================== */

static uint64_t entityHash(const void *item, uint64_t seed0, uint64_t seed1)
{
    tAbelM_entityElem *elem = (tAbelM_entityElem *)item;
    return elem->id;
}

static int entityCompare(const void *a, const void *b, void *udata)
{
    tAbelM_entityElem *elem1 = (tAbelM_entityElem *)a;
    tAbelM_entityElem *elem2 = (tAbelM_entityElem *)b;
    return elem1->id != elem2->id;
}

static ENTITY_ID getNextEntityID()
{
    return AbelM_state.nextEntityID++;
}

static ENTITY_ID addEntity(tAbelE_entity *entity)
{
    ENTITY_ID nextID = getNextEntityID();

    /* add to entity map */
    hashmap_set(AbelM_state.entityMap, &(tAbelM_entityElem){.id = nextID, .entity = entity});
    return nextID;
}

static void rmvEntity(ENTITY_ID id)
{
    hashmap_delete(AbelM_state.entityMap, &(tAbelM_entityElem){.id = id});
}

static tAbelE_entity *getEntity(ENTITY_ID id)
{
    tAbelM_entityElem *elem = hashmap_get(AbelM_state.entityMap, &(tAbelM_entityElem){.id = id});
    return elem ? elem->entity : NULL;
}

ENTITY_ID AbelM_addEntity(tAbelE_entity *entity)
{
    return addEntity(entity);
}

void AbelM_rmvEntity(ENTITY_ID id)
{
    return rmvEntity(id);
}

/* check for collision against other entities */
tAbelE_entity *AbelM_checkEntityCollide(tAbelE_entity *entity)
{
    size_t i = 0;
    void *item;
    tAbelE_entity *other;
    tAbelV_fVec2 pos = AbelE_getPosition(entity), otherPos;
    tAbelV_iVec2 collider = AbelE_getCollider(entity), otherCollider;

    /* walk through each entity, check for collision */
    /* TODO: split entities into 'chunks' */
    while (hashmap_iter(AbelM_state.entityMap, &i, &item)) {
        other = ((tAbelM_entityElem *)item)->entity;
        otherPos = AbelE_getPosition(other);
        otherCollider = AbelE_getCollider(other);

        /* check collide */
        if (pos.x < otherPos.x + otherCollider.x && pos.x + collider.x > otherPos.x && pos.y < otherPos.y + otherCollider.y && pos.y + collider.y > otherPos.y)
            return other;
    }

    /* no collisions found! */
    return NULL;
}

tAbelE_entity *AbelM_getEntity(ENTITY_ID id)
{
    return getEntity(id);
}

void AbelM_renderEntities(void)
{
    size_t i = 0;
    void *item;
    tAbelM_entityElem *elem;

    /* render each entity */
    while (hashmap_iter(AbelM_state.entityMap, &i, &item)) {
        elem = (tAbelM_entityElem *)item;
        AbelE_renderEntity(elem->entity);
    }
}

/* =========================================[[ Chunk ]]========================================= */

static uint64_t chunkHash(const void *item, uint64_t seed0, uint64_t seed1)
{
    tAbelM_chunkElem *elem = (tAbelM_chunkElem *)item;
    int hash = 23 + (seed0 ^ seed1);

    /* do not question the magic numbers */
    hash = (hash * 37) + elem->pos.x;
    hash = (hash * 37) + elem->pos.y;
    return hash;
}

static int chunkCompare(const void *a, const void *b, void *udata)
{
    tAbelM_chunkElem *elem1 = (tAbelM_chunkElem *)a;
    tAbelM_chunkElem *elem2 = (tAbelM_chunkElem *)b;
    return AbelV_compareiVec2(elem1->pos, elem2->pos);
}

static uint32_t worldStepTask(uint32_t delta, void *uData)
{
    size_t i = 0;
    void *item;
    tAbelM_entityElem *elem;

    while (hashmap_iter(AbelM_state.entityMap, &i, &item)) {
        elem = (tAbelM_entityElem *)item;
        AbelE_stepEntity(elem->entity, delta);
    }

    AbelM_state.lastStepTime = SDL_GetTicks();
    return WORLD_STEP_INTERVAL;
}

static uint32_t resetFPSTask(uint32_t delta, void *uData)
{
    AbelM_state.FPS = AbelM_state.currFPS;
    AbelM_state.currFPS = 0;
    return 1000;
}

static tAbelC_chunk *addChunk(tAbelV_iVec2 pos)
{
    tAbelC_chunk *chunk = AbelC_newChunk(pos);

    /* add to chunk map */
    hashmap_set(AbelM_state.chunkMap, &(tAbelM_chunkElem){.pos = pos, .chunk = chunk});
    return chunk;
}

static void rmvChunk(tAbelV_iVec2 pos)
{
    hashmap_delete(AbelM_state.chunkMap, &(tAbelM_chunkElem){.pos = pos});
}

static tAbelC_chunk *getChunk(tAbelV_iVec2 pos)
{
    tAbelM_chunkElem *elem = hashmap_get(AbelM_state.chunkMap, &(tAbelM_chunkElem){.pos = pos});
    return elem ? elem->chunk : NULL;
}

tAbelC_chunk *AbelM_getChunk(tAbelV_iVec2 chunkPos)
{
    return getChunk(chunkPos);
}

tAbelV_iVec2 AbelM_getChunkPos(tAbelV_iVec2 cellPos)
{
    return AbelV_diviVec2(cellPos, AbelC_chunkSize);
}

void AbelM_renderChunks(LAYER_ID layer)
{
    size_t i = 0;
    void *item;
    tAbelM_chunkElem *elem;

    /* render each chunk */
    while (hashmap_iter(AbelM_state.chunkMap, &i, &item)) {
        elem = (tAbelM_chunkElem *)item;
        AbelC_renderChunk(elem->chunk, layer);
    }
}

void AbelM_render(void)
{
    /* clear layers */
    SDL_RenderClear(AbelR_getRenderer());

    /* render chunks */
    AbelM_renderChunks(LAYER_BG);
    AbelM_renderEntities();
    nk_sdl_render(NK_ANTI_ALIASING_ON);

    /* render to window */
    SDL_RenderPresent(AbelR_getRenderer());
    AbelM_state.currFPS++;
}

uint32_t AbelM_getFPS(void)
{
    return AbelM_state.FPS;
}

/* =====================================[[ Initializers ]]====================================== */

void AbelM_init(void)
{
    AbelM_state.entityMap = hashmap_new(sizeof(tAbelM_entityElem), 8, 0, 0, entityHash, entityCompare, NULL, NULL);
    AbelM_state.chunkMap = hashmap_new(sizeof(tAbelM_chunkElem), 4, 0, 0, chunkHash, chunkCompare, NULL, NULL);
    AbelM_state.stepTimer = AbelT_newTask(WORLD_STEP_INTERVAL, worldStepTask, NULL);
    AbelM_state.resetFPSTask = AbelT_newTask(1000, resetFPSTask, NULL);
    AbelM_state.lastStepTime = SDL_GetTicks();
    AbelM_state.FPS = 0;
    AbelM_state.currFPS = 0;
    AbelM_state.nextEntityID = 0;
}

void AbelM_quit(void)
{
    hashmap_free(AbelM_state.entityMap);
    hashmap_free(AbelM_state.chunkMap);

    AbelT_freeTask(AbelM_state.stepTimer);
    AbelT_freeTask(AbelM_state.resetFPSTask);
}

/* =========================================[[ Cells ]]========================================= */

void AbelM_setCell(tAbelV_iVec2 pos, TILE_ID id, bool isSolid)
{
    /* update chunk */
    tAbelV_iVec2 chunkPos = AbelM_getChunkPos(pos), localPos;
    tAbelC_chunk *chunk = AbelM_getChunk(chunkPos);

    /* if chunk doesn't exist, create and add it to chunk map */
    if (chunk == NULL)
        chunk = addChunk(chunkPos);

    localPos = AbelC_globalPosToLocalPos(chunk, pos);
    AbelC_drawTile(chunk, localPos, id, LAYER_BG);
    AbelC_setCell(chunk, localPos, id, isSolid);
}

tAbelM_cell AbelM_getCell(tAbelV_iVec2 pos)
{
    tAbelC_chunk *chunk = AbelM_getChunk(AbelM_getChunkPos(pos));

    /* if chunk doesn't exist, return default cell */
    if (chunk == NULL)
        return (tAbelM_cell){.id = 0, .isSolid = true};

    tAbelV_iVec2 localPos = AbelC_globalPosToLocalPos(chunk, pos);
    return AbelC_getCell(chunk, localPos);
}