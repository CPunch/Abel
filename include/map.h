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

/* setters */
ENTITY_ID AbelM_addEntity(tAbelE_entity *entity);
void AbelM_rmvEntity(ENTITY_ID id);

/* getters */
tAbelE_entity *AbelM_getEntity(ENTITY_ID id);

/* check for collision against other entities */
tAbelE_entity *AbelM_checkEntityCollide(tAbelE_entity *entity);

/* cells */
void AbelM_setCell(tAbel_iVec2 pos, TILE_ID id, bool isSolid);
tAbelM_cell AbelM_getCell(tAbel_iVec2 pos);

#endif