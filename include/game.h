#ifndef ABEL_GAME_H
#define ABEL_GAME_H

#include "abel.h"

typedef enum
{
    LAYER_BG,
    LAYER_ENTITY,
    LAYER_UI,
    LAYER_MAX
} LAYER_TYPE;

/* initializers */
void AbelG_init(void);
void AbelG_quit(void);

tAbelL_layer *AbelG_getLayer(LAYER_TYPE layer);

void AbelG_run(void);

#endif