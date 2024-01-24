#ifndef ABEL_WORLD_H
#define ABEL_WORLD_H

#include "abel.h"
#include "core/event.h"
#include "core/vec2.h"

/* every 10ms, step all world physic events */
#define WORLD_STEP_INTERVAL 10

typedef struct _tAbelW_cell
{
    bool isSolid;
    TILE_ID id;
} tAbelW_cell;

/* initalizers */
void AbelW_init(uint32_t initFlags);
void AbelW_quit(void);
tAbelVM_eventConnection *AbelW_onStepConnect(tEventCallback callback, const void *uData);
tAbelVM_eventConnection **AbelW_onStepHead(void);

/* chunk */
tAbelC_chunk *AbelW_getChunk(tAbelV_iVec2 chunkPos);
tAbelV_iVec2 AbelW_getChunkPos(tAbelV_iVec2 cellPos);
void AbelW_updateActiveChunkPos(tAbelV_iVec2 newChunkPos);
void AbelW_updateActiveDistance(int distance);
void AbelW_render(void);

tAbelV_iVec2 AbelW_gridToPos(tAbelV_iVec2 gridPos);
tAbelV_iVec2 AbelW_posToGrid(tAbelV_iVec2 pos);

/* cells */
void AbelW_setTileSet(tAbelR_texture *tileSet);
void AbelW_setCell(tAbelV_iVec2 pos, TILE_ID id, bool isSolid);
tAbelR_texture *AbelW_getTileSet(void);
tAbelW_cell AbelW_getCell(tAbelV_iVec2 pos);

/* entities */
void AbelW_addEntity(tAbelE_entity *entity);
void AbelW_rmvEntity(tAbelE_entity *entity);
void AbelW_updateEntityList(void);

#endif