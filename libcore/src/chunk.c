#include "chunk.h"

#include "assets.h"
#include "core/mem.h"
#include "core/serror.h"
#include "core/vec2.h"
#include "entity.h"
#include "render.h"
#include "sprite.h"
#include "world.h"

tAbelV_iVec2 AbelC_chunkSize = {16, 16};

static int findEmptyEntityIndex(tAbelC_chunk *chunk)
{
    for (int i = 0; i < AbelM_countVector(chunk->entities); i++) {
        if (chunk->entities[i] == NULL)
            return i;
    }

    return -1;
}

/* =======================================[[ Chunk API ]]======================================= */

tAbelC_chunk *AbelC_newChunk(tAbelR_texture *tileSet, tAbelV_iVec2 position)
{
    tAbelC_chunk *chunk = (tAbelC_chunk *)AbelM_malloc(sizeof(tAbelC_chunk));
    tAbelV_iVec2 textureSize = AbelV_muliVec2(AbelC_chunkSize, tileSet->tileSize);

    chunk->cellMap = (tAbelW_cell *)AbelM_malloc(sizeof(tAbelW_cell) * AbelC_chunkSize.x * AbelC_chunkSize.y);
    chunk->tileSet = tileSet;
    chunk->pos = position;

    AbelM_initVector(chunk->entities, 4);
    AbelR_retainTexture(chunk->tileSet);
    return chunk;
}

void AbelC_freeChunk(tAbelC_chunk *chunk)
{
    tAbelE_entity *entity;
    for (int i = 0; i < AbelM_countVector(chunk->entities); i++) {
        if ((entity = chunk->entities[i])) {
            AbelM_releaseRef(&entity->refCount);
        }
    }

    AbelR_releaseTexture(chunk->tileSet);
    AbelM_free(chunk->cellMap);
    AbelM_free(chunk->entities);
    AbelM_free(chunk);
}

/* ========================================[[ Drawing ]]======================================== */

static void drawTileClip(tAbelC_chunk *chunk, int id, tAbelV_iVec2 cellPos)
{
    SDL_Rect dest, src;
    tAbelV_iVec2 offset, scale;
    int posX, posY;

    if (id == TILE_NIL)
        return; /* do nothing */

    /* get camera offset and scale */
    offset = AbelR_getCameraOffset();
    scale = AbelR_getScale();

    /* get tileset clip */
    src = AbelR_getTileClip(chunk->tileSet, id);

    /* get clip of render target */
    posX = (chunk->pos.x * AbelC_chunkSize.x) + cellPos.x;
    posY = (chunk->pos.y * AbelC_chunkSize.y) + cellPos.y;
    dest = (SDL_Rect){.x = (posX * chunk->tileSet->tileSize.x) * scale.x + offset.x,
                      .y = (posY * chunk->tileSet->tileSize.y) * scale.y + offset.y,
                      .w = chunk->tileSet->tileSize.x * scale.x,
                      .h = chunk->tileSet->tileSize.y * scale.y};

    /* draw clipped tile */
    SDL_RenderCopy(AbelR_getRenderer(), chunk->tileSet->texture, &src, &dest);
}

/* renders selected layer into AbelR_getRenderer() */
void AbelC_renderChunk(tAbelC_chunk *chunk, LAYER_ID layer)
{
    for (int y = 0; y < AbelC_chunkSize.y; y++) {
        for (int x = 0; x < AbelC_chunkSize.x; x++) {
            tAbelW_cell cell = AbelC_getCell(chunk, AbelV_newiVec2(x, y));
            if (layer == LAYER_BG) {
                drawTileClip(chunk, cell.bgID, AbelV_newiVec2(x, y));
            } else if (layer == LAYER_FG) {
                drawTileClip(chunk, cell.fgID, AbelV_newiVec2(x, y));
            }
        }
    }

    /* draw red debug chunk line */
    // SDL_SetRenderDrawColor(AbelR_getRenderer(), 255, 0, 0, 255);
    // SDL_RenderDrawLine(AbelR_getRenderer(), dest.x, dest.y, dest.x + dest.w, dest.y);
    // SDL_RenderDrawLine(AbelR_getRenderer(), dest.x, dest.y, dest.x, dest.y + dest.h);
    // SDL_SetRenderDrawColor(AbelR_getRenderer(), 0, 0, 0, 255);
}

/* =========================================[[ Cells ]]========================================= */

void AbelC_setCell(tAbelC_chunk *chunk, tAbelV_iVec2 pos, TILE_ID bg, TILE_ID fg, bool isSolid)
{
    /* validation */
    int indx = (AbelC_chunkSize.x * pos.y) + pos.x;
    if (indx >= AbelC_chunkSize.x * AbelC_chunkSize.y)
        return; /* do nothing */

    chunk->cellMap[indx] = (tAbelW_cell){.fgID = fg, .bgID = bg, .isSolid = isSolid};
}

tAbelW_cell AbelC_getCell(tAbelC_chunk *chunk, tAbelV_iVec2 pos)
{
    /* validation */
    int indx = (AbelC_chunkSize.x * pos.y) + pos.x;
    if (indx >= AbelC_chunkSize.x * AbelC_chunkSize.y)
        return (tAbelW_cell){.fgID = TILE_NIL, .bgID = TILE_NIL, .isSolid = true}; /* if cell doesn't exist, return default cell */

    return chunk->cellMap[indx];
}

void AbelC_addEntity(tAbelC_chunk *chunk, tAbelE_entity *entity)
{
    int indx = findEmptyEntityIndex(chunk);
    if (indx == -1) {
        AbelM_growVector(tAbelE_entity *, chunk->entities, 1);
        chunk->entities[AbelM_countVector(chunk->entities)++] = entity;
    } else {
        chunk->entities[indx] = entity;
    }

    AbelM_retainRef(&entity->refCount);
}

void AbelC_rmvEntity(tAbelC_chunk *chunk, tAbelE_entity *entity)
{
    for (int i = 0; i < AbelM_countVector(chunk->entities); i++) {
        if (chunk->entities[i] == entity) {
            chunk->entities[i] = NULL;
            AbelM_releaseRef(&entity->refCount);
            return;
        }
    }
}

void AbelC_stepEntities(tAbelC_chunk *chunk, uint32_t delta)
{
    tAbelE_entity *entity;
    for (int i = 0; i < AbelM_countVector(chunk->entities); i++) {
        if ((entity = chunk->entities[i])) {
            AbelE_stepEntity(entity, delta);
        }
    }
}

/* note: returned pointer is only valid until entities are modified.
    make sure to check the returned array for NULL ptrs! */
tAbelE_entity **AbelC_getEntities(tAbelC_chunk *chunk, size_t *size)
{
    *size = AbelM_countVector(chunk->entities);
    return chunk->entities;
}

/* =========================================[[ Utils ]]========================================= */

tAbelV_iVec2 AbelC_gridPosToLocalPos(tAbelC_chunk *chunk, tAbelV_iVec2 gridPos)
{
    tAbelV_iVec2 chunkGridPos = AbelV_muliVec2(chunk->pos, AbelC_chunkSize);
    ABEL_ASSERT(gridPos.x >= 0 && gridPos.y >= 0);

    return AbelV_subiVec2(gridPos, chunkGridPos);
}