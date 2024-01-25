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
    tAbelW_cell *cellMap;
    tAbelR_texture *tileSet;
    AbelM_newVector(tAbelE_entity *, entities);
    tAbelV_iVec2 pos;
    struct _tAbelC_chunk *nextActive;
} tAbelC_chunk;

#define CHUNK_VIEW_DIST 2

/* chunks */
tAbelC_chunk *AbelC_newChunk(tAbelR_texture *tileSet, tAbelV_iVec2 position);
void AbelC_freeChunk(tAbelC_chunk *chunk);

void AbelC_renderChunk(tAbelC_chunk *chunk, LAYER_ID layer);

/* cells */
void AbelC_setCell(tAbelC_chunk *chunk, tAbelV_iVec2 pos, TILE_ID bg, TILE_ID fg, bool isSolid);
tAbelW_cell AbelC_getCell(tAbelC_chunk *chunk, tAbelV_iVec2 pos);

/* entities */
void AbelC_addEntity(tAbelC_chunk *chunk, tAbelE_entity *entity);
void AbelC_rmvEntity(tAbelC_chunk *chunk, tAbelE_entity *entity);
void AbelC_stepEntities(tAbelC_chunk *chunk, uint32_t delta);

/* note: returned pointer is only valid until entities are modified.
    make sure to check the returned array for NULL ptrs! */
tAbelE_entity **AbelC_getEntities(tAbelC_chunk *chunk, size_t *size);

/* utils */
tAbelV_iVec2 AbelC_gridPosToLocalPos(tAbelC_chunk *chunk, tAbelV_iVec2 gridPos);

#endif