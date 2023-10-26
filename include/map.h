#ifndef ABEL_MAP_H
#define ABEL_MAP_H

#include "abel.h"
#include "core/vec2.h"

/* every 50ms, step all world physic events */
#define WORLD_STEP_INTERVAL 10

typedef struct _tAbelM_cell
{
    bool isSolid;
    TILE_ID id;
} tAbelM_cell;

/* initalizers */
void AbelM_init(void);
void AbelM_quit(void);

/* entity */
ENTITY_ID AbelM_addEntity(tAbelE_entity *entity);
void AbelM_rmvEntity(ENTITY_ID id);

tAbelE_entity *AbelM_checkEntityCollide(tAbelE_entity *entity);
tAbelE_entity *AbelM_getEntity(ENTITY_ID id);

void AbelM_renderEntities(void);

/* chunk */
tAbelC_chunk* AbelM_getChunk(tAbelV_iVec2 chunkPos);
tAbelV_iVec2 AbelM_getChunkPos(tAbelV_iVec2 cellPos);
void AbelM_renderChunks(LAYER_ID layer);

/* cells */
void AbelM_setCell(tAbelV_iVec2 pos, tAbelR_texture *tileSet, TILE_ID id, bool isSolid);
tAbelM_cell AbelM_getCell(tAbelV_iVec2 pos);

#endif