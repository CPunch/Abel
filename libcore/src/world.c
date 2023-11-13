#include "world.h"

#include "chunk.h"
#include "core/hashmap.h"
#include "core/mem.h"
#include "core/tasks.h"
#include "entity.h"
#include "game.h"
#include "render.h"

typedef struct
{
    tAbelV_iVec2 pos;
    tAbelC_chunk *chunk;
} tAbelW_chunkElem;

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

typedef struct _tAbelW_state
{
    tAbelT_task *stepTimer;
    struct hashmap *chunkMap;
    tAbelE_entity *renderHead;
    uint32_t lastStepTime;
} tAbelW_state;

static tAbelW_state AbelW_state = {0};

/* =====================================[[ Initializers ]]====================================== */

static uint32_t worldStepTask(uint32_t delta, void *uData)
{
    size_t i = 0;
    void *item;
    tAbelW_chunkElem *elem;

    while (hashmap_iter(AbelW_state.chunkMap, &i, &item)) {
        elem = (tAbelW_chunkElem *)item;
        AbelC_stepEntities(elem->chunk, delta);
    }

    AbelW_state.lastStepTime = SDL_GetTicks();
    return WORLD_STEP_INTERVAL;
}

void AbelW_init(void)
{
    AbelW_state.chunkMap = hashmap_new(sizeof(tAbelW_chunkElem), 4, 0, 0, chunkHash, chunkCompare, NULL, NULL);
    AbelW_state.stepTimer = AbelT_newTask(WORLD_STEP_INTERVAL, worldStepTask, NULL);
    AbelW_state.lastStepTime = SDL_GetTicks();
    AbelW_state.renderHead = NULL;
}

void AbelW_quit(void)
{
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

/* ===================================[[ Helper Functions ]]==================================== */

/* TODO: move away from using the hashmap. we should compute visible chunks beforehand
 and store them in a list. this way we can iterate through the list and render each chunk */

#define forEachChunk(chunkIdentifier, ...)                                                                                                                     \
    {                                                                                                                                                          \
        size_t i = 0;                                                                                                                                          \
        void *item;                                                                                                                                            \
        tAbelW_chunkElem *elem;                                                                                                                                \
        tAbelC_chunk *chunkIdentifier;                                                                                                                         \
                                                                                                                                                               \
        while (hashmap_iter(AbelW_state.chunkMap, &i, &item)) {                                                                                                \
            chunkIdentifier = ((tAbelW_chunkElem *)item)->chunk;                                                                                               \
            __VA_ARGS__                                                                                                                                        \
        }                                                                                                                                                      \
    }

#define forEachEntity(entityIdentifier, ...)                                                                                                                   \
    {                                                                                                                                                          \
        forEachChunk(chunk, {                                                                                                                                  \
            size_t sz;                                                                                                                                         \
            tAbelE_entity **entities = AbelC_getEntities(chunk, &sz);                                                                                          \
            tAbelE_entity *entityIdentifier;                                                                                                                   \
                                                                                                                                                               \
            for (int i = 0; i < sz; i++) {                                                                                                                     \
                if (entities[i] == NULL)                                                                                                                       \
                    continue;                                                                                                                                  \
                entityIdentifier = entities[i];                                                                                                                \
                __VA_ARGS__                                                                                                                                    \
            }                                                                                                                                                  \
        })                                                                                                                                                     \
    }

static void clearRenderOrder()
{
    forEachEntity(entity, { entity->renderNext = NULL; });

    AbelW_state.renderHead = NULL;
}

/* sort every entity from every chunk */
static void recomputeRenderOrder()
{
    clearRenderOrder();

    forEachEntity(entity, {
        tAbelE_entity *curr = AbelW_state.renderHead, *last = NULL;

        /* search linked list for insert location */
        while (curr && curr->sprite.pos.y < entity->sprite.pos.y) {
            last = curr;
            curr = curr->renderNext;
        }

        /* insert entity */
        if (curr == AbelW_state.renderHead) {
            entity->renderNext = AbelW_state.renderHead;
            AbelW_state.renderHead = entity;
        } else {
            entity->renderNext = curr;
            last->renderNext = entity;
        }
    });
}

/* =========================================[[ Chunk ]]========================================= */

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

void AbelW_renderChunks()
{
    size_t i = 0, sz;
    void *item;
    tAbelC_chunk *chunk;

    /* render each chunk */
    while (hashmap_iter(AbelW_state.chunkMap, &i, &item)) {
        tAbelE_entity **entities;
        tAbelE_entity *entity;

        chunk = ((tAbelW_chunkElem *)item)->chunk;
        entities = AbelC_getEntities(chunk, &sz);

        /* render BG, each entity & FG */
        AbelC_renderChunk(chunk, LAYER_BG);
        // TODO: render entities
        AbelC_renderChunk(chunk, LAYER_FG);
    }
}

static tAbelC_chunk *grabChunk(tAbelV_iVec2 chunkPos)
{
    tAbelC_chunk *chunk = getChunk(chunkPos);

    /* if chunk doesn't exist, create and add it to chunk map */
    if (chunk == NULL)
        chunk = addChunk(chunkPos);

    return chunk;
}

/* =========================================[[ Cells ]]========================================= */

void AbelW_setCell(tAbelV_iVec2 pos, TILE_ID id, bool isSolid)
{
    /* update chunk */
    tAbelV_iVec2 chunkPos = AbelW_getChunkPos(pos), localPos;
    tAbelC_chunk *chunk = grabChunk(chunkPos);

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

/* =========================================[[ Entity ]]======================================== */

void AbelW_addEntity(tAbelE_entity *entity)
{
    tAbelV_iVec2 gridPos = AbelC_posToGrid(AbelV_f2iVec(entity->sprite.pos));
    tAbelV_iVec2 chunkPos = AbelW_getChunkPos(gridPos);
    tAbelC_chunk *chunk = grabChunk(chunkPos);

    AbelE_setChunk(entity, chunk);
}

void AbelW_rmvEntity(tAbelE_entity *entity)
{
    AbelE_setChunk(entity, NULL);
}

#undef forEachEntity
#undef forEachChunk