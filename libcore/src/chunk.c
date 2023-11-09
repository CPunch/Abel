#include "chunk.h"

#include "assets.h"
#include "core/mem.h"
#include "core/serror.h"
#include "core/vec2.h"
#include "render.h"
#include "sprite.h"
#include "world.h"

tAbelV_iVec2 AbelC_chunkSize = AbelV_newiVec2(16, 16);

/* =======================================[[ Chunk API ]]======================================= */

tAbelC_chunk *AbelC_newChunk(tAbelV_iVec2 position)
{
    tAbelC_chunk *chunk = (tAbelC_chunk *)AbelM_malloc(sizeof(tAbelC_chunk));
    tAbelV_iVec2 textureSize = AbelV_muliVec2(AbelC_chunkSize, AbelR_tileSize);

    chunk->bgFrame = AbelR_newBlankTexture(textureSize);
    chunk->fgFrame = AbelR_newBlankTexture(textureSize);
    chunk->cellMap = (tAbelW_cell *)AbelM_malloc(sizeof(tAbelW_cell) * AbelC_chunkSize.x * AbelC_chunkSize.y);
    chunk->pos = position;
    return chunk;
}

void AbelC_freeChunk(tAbelC_chunk *chunk)
{
    AbelR_freeTexture(chunk->bgFrame);
    AbelR_freeTexture(chunk->fgFrame);
    AbelM_free(chunk->cellMap);
    AbelM_free(chunk);
}

/* ========================================[[ Drawing ]]======================================== */

static void drawTileClip(tAbelC_chunk *chunk, int id, tAbelV_iVec2 pos, LAYER_ID layer)
{
    SDL_Rect dest, src;
    tAbelR_texture *tileSet = AbelA_getTexture(ASSET_ID_MAP_TILESET);

    /* get tileset clip */
    src = AbelR_getTileClip(tileSet, id);

    /* get clip of render target */
    dest = (SDL_Rect){.x = pos.x * AbelR_tileSize.x, .y = pos.y * AbelR_tileSize.y, .w = AbelR_tileSize.x, .h = AbelR_tileSize.y};

    /* render */
    switch (layer) {
    case LAYER_BG:
        SDL_SetRenderTarget(AbelR_getRenderer(), chunk->bgFrame->texture);
        break;
    case LAYER_FG:
        SDL_SetRenderTarget(AbelR_getRenderer(), chunk->fgFrame->texture);
        break;
    default:
        ABEL_ERROR("Invalid layer id: %d\n", layer);
    }

    /* draw clipped texture */
    AbelR_renderTexture(tileSet, &src, &dest);
    SDL_SetRenderTarget(AbelR_getRenderer(), NULL);
}

/* renders selected layer into AbelR_getRenderer() */
void AbelC_renderChunk(tAbelC_chunk *chunk, LAYER_ID layer)
{
    SDL_Rect dest, src;
    tAbelV_iVec2 offset = AbelR_getCameraOffset(), scale = AbelR_getScale();

    src = (SDL_Rect){.x = 0, .y = 0, .w = AbelC_chunkSize.x * AbelR_tileSize.x, .h = AbelC_chunkSize.y * AbelR_tileSize.y};

    /* get clip of render target */
    dest = (SDL_Rect){.x = (((chunk->pos.x * AbelR_tileSize.x) * AbelC_chunkSize.x) * scale.x) + offset.x,
                      .y = (((chunk->pos.y * AbelR_tileSize.y) * AbelC_chunkSize.y) * scale.y) + offset.y,
                      .w = AbelR_tileSize.x * AbelC_chunkSize.x * scale.x,
                      .h = AbelR_tileSize.y * AbelC_chunkSize.y * scale.y};

    /* render */
    switch (layer) {
    case LAYER_BG:
        AbelR_renderTexture(chunk->bgFrame, &src, &dest);
        break;
    case LAYER_FG:
        AbelR_renderTexture(chunk->fgFrame, &src, &dest);
        break;
    default:
        ABEL_ERROR("Invalid layer id: %d\n", layer);
    }

    /* draw red debug chunk line */
    SDL_SetRenderDrawColor(AbelR_getRenderer(), 255, 0, 0, 255);
    SDL_RenderDrawLine(AbelR_getRenderer(), dest.x, dest.y, dest.x + dest.w, dest.y);
    SDL_RenderDrawLine(AbelR_getRenderer(), dest.x, dest.y, dest.x, dest.y + dest.h);
    SDL_SetRenderDrawColor(AbelR_getRenderer(), 0, 0, 0, 255);
}

void AbelC_drawTile(tAbelC_chunk *chunk, tAbelV_iVec2 pos, TILE_ID id, LAYER_ID layer)
{
    drawTileClip(chunk, id, pos, layer);
}

void AbelC_setCell(tAbelC_chunk *chunk, tAbelV_iVec2 pos, TILE_ID id, bool isSolid)
{
    /* validation */
    int indx = (AbelC_chunkSize.x * pos.y) + pos.x;
    if (indx >= AbelC_chunkSize.x * AbelC_chunkSize.y)
        return; /* do nothing */

    chunk->cellMap[indx] = (tAbelW_cell){.id = id, .isSolid = isSolid};
}

tAbelW_cell AbelC_getCell(tAbelC_chunk *chunk, tAbelV_iVec2 pos)
{
    /* validation */
    int indx = (AbelC_chunkSize.x * pos.y) + pos.x;
    if (indx >= AbelC_chunkSize.x * AbelC_chunkSize.y)
        return (tAbelW_cell){.id = 0, .isSolid = true}; /* if cell doesn't exist, return default cell */

    return chunk->cellMap[indx];
}

/* =========================================[[ Utils ]]========================================= */

tAbelV_iVec2 AbelC_gridToPos(tAbelV_iVec2 gridPos)
{
    return AbelV_muliVec2(gridPos, AbelR_tileSize);
}

tAbelV_iVec2 AbelC_posToGrid(tAbelV_iVec2 pos)
{
    return AbelV_diviVec2(pos, AbelR_tileSize);
}

tAbelV_iVec2 AbelC_worldPosToLocalPos(tAbelC_chunk *chunk, tAbelV_iVec2 worldPos)
{
    return AbelV_subiVec2(worldPos, AbelV_muliVec2(chunk->pos, AbelC_chunkSize));
}