#ifndef ABEL_WORLD_H
#define ABEL_WORLD_H

#include "abel.h"
#include "core/vec2.h"

/* every 50ms, step all world physic events */
#define WORLD_STEP_INTERVAL 10

typedef struct _tAbelW_cell
{
    bool isSolid;
    TILE_ID id;
} tAbelW_cell;

/* initalizers */
void AbelW_init(void);
void AbelW_quit(void);

/* chunk */
tAbelC_chunk *AbelW_getChunk(tAbelV_iVec2 chunkPos);
tAbelV_iVec2 AbelW_getChunkPos(tAbelV_iVec2 cellPos);
void AbelW_renderChunks();

/* cells */
void AbelW_setCell(tAbelV_iVec2 pos, TILE_ID id, bool isSolid);
tAbelW_cell AbelW_getCell(tAbelV_iVec2 pos);

/* entities */
void AbelW_addEntity(tAbelE_entity *entity);
void AbelW_rmvEntity(tAbelE_entity *entity);

#endif