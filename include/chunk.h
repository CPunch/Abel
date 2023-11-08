#ifndef ABEL_CHUNK_H
#define ABEL_CHUNK_H

#include "abel.h"
#include "core/hashmap.h"
#include "core/mem.h"
#include "core/vec2.h"

extern tAbelV_iVec2 AbelC_chunkSize;

enum _LAYER_ID
{
    LAYER_BG,
    LAYER_FG
};

typedef struct _tAbelC_chunk
{
    tAbelR_texture *bgFrame;
    tAbelR_texture *fgFrame;
    tAbelW_cell *cellMap;
    tAbelV_iVec2 pos;
} tAbelC_chunk;

/* chunks */
tAbelC_chunk *AbelC_newChunk(tAbelV_iVec2 position);
void AbelC_freeChunk(tAbelC_chunk *chunk);

/* drawing */
void AbelC_renderChunk(tAbelC_chunk *chunk, LAYER_ID layer);
void AbelC_drawTile(tAbelC_chunk *chunk, tAbelV_iVec2 pos, TILE_ID id, LAYER_ID layer);
void AbelC_setCell(tAbelC_chunk *chunk, tAbelV_iVec2 pos, TILE_ID id, bool isSolid);
tAbelW_cell AbelC_getCell(tAbelC_chunk *chunk, tAbelV_iVec2 pos);

/* utils */
tAbelV_iVec2 AbelC_gridToPos(tAbelV_iVec2 gridPos);
tAbelV_iVec2 AbelC_posToGrid(tAbelV_iVec2 pos);
tAbelV_iVec2 AbelC_worldPosToLocalPos(tAbelC_chunk *chunk, tAbelV_iVec2 worldPos);

#endif