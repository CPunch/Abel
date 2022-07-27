#ifndef ABEL_WORLD_H
#define ABEL_WORLD_H

#include "abel.h"

/* every 50ms, step all world physic events */
#define WORLD_STEP_INTERVAL 50

/* initalizers */
void AbelW_init(void);
void AbelW_quit(void);

/* setters */
ENTITY_ID AbelW_addEntity(tAbelE_entity *entity);
void AbelW_rmvEntity(ENTITY_ID id);

/* getters */
tAbelE_entity *AbelW_getEntity(ENTITY_ID id);

#endif