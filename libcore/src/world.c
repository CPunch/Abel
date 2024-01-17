#include "world.h"

#include "chunk.h"
#include "core/event.h"
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

static tAbelC_chunk *getChunk(tAbelV_iVec2 pos);

typedef struct _tAbelW_state
{
    tAbelR_texture *tileSet; /* default world tileset to set chunks too */
    tAbelT_task *stepTimer;
    tAbelVM_eventConnection *onStep;
    struct hashmap *chunkMap;
    tAbelE_entity *renderHead; /* sorted linked list of entities to render in order */
    tAbelC_chunk *activeHead;  /* linked list of currently visible chunks */
    tAbelV_iVec2 activeChunkPos;
    int activeDist;
    uint32_t lastStepTime;
} tAbelW_state;

static tAbelW_state AbelW_state = {0};

/* =====================================[[ Initializers ]]====================================== */

static uint32_t worldStepTask(uint32_t delta, void *uData)
{
    AbelVM_fireEventList(AbelW_state.onStep, NULL);

    /* step all active chunks */
    for (tAbelC_chunk *curr = AbelW_state.activeHead; curr; curr = curr->nextActive) {
        AbelC_stepEntities(curr, delta);
    }

    AbelW_state.lastStepTime = SDL_GetTicks();
    return WORLD_STEP_INTERVAL;
}

static void reset()
{
    AbelW_state.tileSet = NULL;
    AbelW_state.stepTimer = NULL;
    AbelW_state.onStep = NULL;
    AbelW_state.chunkMap = NULL;
    AbelW_state.renderHead = NULL;
    AbelW_state.activeHead = NULL;
    AbelW_state.activeChunkPos = AbelV_newiVec2(0, 0);
    AbelW_state.activeDist = 1;
    AbelW_state.lastStepTime = SDL_GetTicks();
}

void AbelW_init(void)
{
    reset();
    AbelW_state.stepTimer = AbelT_newTask(WORLD_STEP_INTERVAL, worldStepTask, NULL);
    AbelW_state.chunkMap = hashmap_new(sizeof(tAbelW_chunkElem), 4, 0, 0, chunkHash, chunkCompare, NULL, NULL);
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
    AbelVM_clearEventList(&AbelW_state.onStep);
    AbelT_freeTask(AbelW_state.stepTimer);

    if (AbelW_state.tileSet)
        AbelR_releaseTexture(AbelW_state.tileSet);

    /* asan won't actually mark any globally stored pointers as
     memory leaks */
    reset();
}

tAbelVM_eventConnection *AbelW_onStepConnect(tEventCallback callback, const void *uData)
{
    return AbelVM_connectEvent(&AbelW_state.onStep, callback, uData);
}

tAbelVM_eventConnection **AbelW_onStepHead(void)
{
    return &AbelW_state.onStep;
}

/* ===================================[[ Helper Functions ]]==================================== */

static void insertRenderOrder(tAbelE_entity *entity)
{
    tAbelE_entity *curr = AbelW_state.renderHead, *last = NULL;

    /* search linked list for insert location */
    while (curr && curr->sprite.pos.y < entity->sprite.pos.y) {
        last = curr;
        curr = curr->nextRender;
    }

    /* insert entity */
    if (curr == AbelW_state.renderHead) {
        entity->nextRender = AbelW_state.renderHead;
        AbelW_state.renderHead = entity;
    } else {
        entity->nextRender = curr;
        last->nextRender = entity;
    }
}

static void clearRenderOrder()
{
    AbelW_state.renderHead = NULL;
}

/* sort every entity from every chunk */
static void recomputeEntityRenderOrder()
{
    size_t chunkSz;
    tAbelE_entity **entities;
    tAbelE_entity *entity;

    clearRenderOrder();
    for (tAbelC_chunk *curr = AbelW_state.activeHead; curr; curr = curr->nextActive) {
        entities = AbelC_getEntities(curr, &chunkSz);
        for (int i = 0; i < chunkSz; i++) {
            entity = entities[i];
            if (entity == NULL)
                continue;

            insertRenderOrder(entity);
        }
    }
}

static void insertActiveChunk(tAbelC_chunk *chunk)
{
    chunk->nextActive = AbelW_state.activeHead;
    AbelW_state.activeHead = chunk;
}

static void removeActiveChunk(tAbelC_chunk *chunk)
{
    tAbelC_chunk *curr = AbelW_state.activeHead, *last = NULL;

    /* look for visible chunk */
    while (curr && curr != chunk) {
        last = curr;
        curr = curr->nextActive;
    }

    /* chunk is already removed */
    if (curr == NULL) {
        chunk->nextActive = NULL;
        return;
    }

    /* remove from list */
    if (last)
        last->nextActive = chunk->nextActive;
    chunk->nextActive = NULL;
}

static void clearActiveChunks()
{
    AbelW_state.activeHead = NULL;
}

static void recomputeActiveChunks(tAbelV_iVec2 newChunkPos, int activeDist)
{
    tAbelC_chunk *chunk;

    clearActiveChunks();
    for (int x = -activeDist; x <= activeDist; x++) {
        for (int y = -activeDist; y <= activeDist; y++) {
            if ((chunk = getChunk(AbelV_newiVec2(newChunkPos.x + x, newChunkPos.y + y)))) {
                insertActiveChunk(chunk);
            }
        }
    }

    recomputeEntityRenderOrder();
    AbelW_state.activeChunkPos = newChunkPos;
    AbelW_state.activeDist = activeDist;
}

static void checkChunkUpdate(tAbelV_iVec2 chunkPos)
{
    // printf("comparing (%d, %d) render dist [%d] against (%d, %d)\n", AbelW_state.activeChunkPos.x, AbelW_state.activeChunkPos.y, AbelW_state.activeDist,
    // chunkPos.x, chunkPos.y);
    /* if chunkPos is within our visible range, we'll need to update things */
    if (AbelW_state.activeChunkPos.x - AbelW_state.activeDist <= chunkPos.x && AbelW_state.activeChunkPos.x + AbelW_state.activeDist >= chunkPos.x &&
        AbelW_state.activeChunkPos.y - AbelW_state.activeDist <= chunkPos.y && AbelW_state.activeChunkPos.y + AbelW_state.activeDist >= chunkPos.y) {
        recomputeActiveChunks(AbelW_state.activeChunkPos, AbelW_state.activeDist);
    }
}

/* =========================================[[ Chunk ]]========================================= */

static tAbelC_chunk *addChunk(tAbelV_iVec2 pos)
{
    ABEL_ASSERT(AbelW_state.tileSet);
    tAbelC_chunk *chunk = AbelC_newChunk(AbelW_state.tileSet, pos);

    /* add to chunk map */
    hashmap_set(AbelW_state.chunkMap, &(tAbelW_chunkElem){.pos = pos, .chunk = chunk});

    checkChunkUpdate(pos);
    return chunk;
}

static void rmvChunk(tAbelV_iVec2 pos)
{
    hashmap_delete(AbelW_state.chunkMap, &(tAbelW_chunkElem){.pos = pos});

    checkChunkUpdate(pos);
}

static tAbelC_chunk *getChunk(tAbelV_iVec2 pos)
{
    tAbelW_chunkElem *elem = hashmap_get(AbelW_state.chunkMap, &(tAbelW_chunkElem){.pos = pos});
    return elem ? elem->chunk : NULL;
}

static tAbelC_chunk *grabChunk(tAbelV_iVec2 chunkPos)
{
    tAbelC_chunk *chunk = getChunk(chunkPos);

    /* if chunk doesn't exist, create and add it to chunk map */
    if (chunk == NULL)
        chunk = addChunk(chunkPos);

    return chunk;
}

tAbelC_chunk *AbelW_getChunk(tAbelV_iVec2 chunkPos)
{
    return grabChunk(chunkPos);
}

tAbelV_iVec2 AbelW_getChunkPos(tAbelV_iVec2 cellPos)
{
    tAbelV_iVec2 chunkPos = AbelV_diviVec2(cellPos, AbelC_chunkSize);
    if (cellPos.x < 0)
        chunkPos.x--;
    if (cellPos.y < 0)
        chunkPos.y--;
    return chunkPos;
}

void AbelW_updateActiveChunkPos(tAbelV_iVec2 newChunkPos)
{
    /* if our last update was the same, no need to update everything */
    if (!AbelV_compareiVec2(newChunkPos, AbelW_state.activeChunkPos))
        return;

    recomputeActiveChunks(newChunkPos, AbelW_state.activeDist);
}

void AbelW_updateActiveDistance(int distance)
{
    /* if our last update was the same, no need to update everything */
    if (AbelW_state.activeDist == distance)
        return;

    recomputeActiveChunks(AbelW_state.activeChunkPos, distance);
}

void AbelW_render()
{
    for (tAbelC_chunk *curr = AbelW_state.activeHead; curr; curr = curr->nextActive) {
        AbelC_renderChunk(curr, LAYER_BG);
    }

    for (tAbelE_entity *curr = AbelW_state.renderHead; curr; curr = curr->nextRender) {
        AbelE_renderEntity(curr);
    }

    for (tAbelC_chunk *curr = AbelW_state.activeHead; curr; curr = curr->nextActive) {
        AbelC_renderChunk(curr, LAYER_FG);
    }
}

tAbelV_iVec2 AbelW_gridToPos(tAbelV_iVec2 gridPos)
{
    ABEL_ASSERT(AbelW_state.tileSet);
    return AbelV_muliVec2(gridPos, AbelW_state.tileSet->tileSize);
}

tAbelV_iVec2 AbelW_posToGrid(tAbelV_iVec2 pos)
{
    ABEL_ASSERT(AbelW_state.tileSet);
    tAbelV_iVec2 grid = AbelV_diviVec2(pos, AbelW_state.tileSet->tileSize);

    if (pos.x < 0)
        grid.x--;
    if (pos.y < 0)
        grid.y--;

    return grid;
}

/* =========================================[[ Cells ]]========================================= */

void AbelW_setTileSet(tAbelR_texture *tileSet)
{
    if (AbelW_state.tileSet)
        AbelR_releaseTexture(AbelW_state.tileSet);

    if (tileSet)
        AbelR_retainTexture(tileSet);

    AbelW_state.tileSet = tileSet;
}

void AbelW_setCell(tAbelV_iVec2 pos, TILE_ID id, bool isSolid)
{
    /* update chunk */
    tAbelV_iVec2 chunkPos = AbelW_getChunkPos(pos), localPos;
    tAbelC_chunk *chunk = grabChunk(chunkPos);

    localPos = AbelC_gridPosToLocalPos(chunk, pos);
    AbelC_drawTile(chunk, localPos, id, LAYER_BG);
    AbelC_setCell(chunk, localPos, id, isSolid);
}

tAbelR_texture *AbelW_getTileSet(void)
{
    ABEL_ASSERT(AbelW_state.tileSet);
    return AbelW_state.tileSet;
}

tAbelW_cell AbelW_getCell(tAbelV_iVec2 pos)
{
    tAbelC_chunk *chunk = getChunk(AbelW_getChunkPos(pos));

    /* if chunk doesn't exist, return default cell */
    if (chunk == NULL)
        return (tAbelW_cell){.id = 0, .isSolid = true};

    tAbelV_iVec2 localPos = AbelC_gridPosToLocalPos(chunk, pos);
    return AbelC_getCell(chunk, localPos);
}

/* =========================================[[ Entity ]]======================================== */

void AbelW_addEntity(tAbelE_entity *entity)
{
    tAbelV_iVec2 gridPos = AbelW_posToGrid(AbelV_f2iVec(entity->sprite.pos));
    tAbelV_iVec2 chunkPos = AbelW_getChunkPos(gridPos);
    tAbelC_chunk *chunk = grabChunk(chunkPos);

    AbelE_setChunk(entity, chunk);
    checkChunkUpdate(chunkPos);
}

void AbelW_rmvEntity(tAbelE_entity *entity)
{
    tAbelV_iVec2 gridPos = AbelW_posToGrid(AbelV_f2iVec(entity->sprite.pos));
    tAbelV_iVec2 chunkPos = AbelW_getChunkPos(gridPos);

    AbelE_setChunk(entity, NULL);
    checkChunkUpdate(chunkPos);
}
