#ifndef ABEL_LAYER_H
#define ABEL_LAYER_H

#include "abel.h"
#include "core/hashmap.h"
#include "vec2.h"

#define TILESET_SIZE 16

typedef struct _tAbelL_layer
{
    tAbelR_texture *tileSet;
    tAbelR_texture *frame;
} tAbelL_layer;

tAbelL_layer *AbelL_newLayer(tAbelR_texture *tileSet, tAbel_vec2 size);
void AbelL_freeLayer(tAbelL_layer *tileSet);

void AbelL_renderTileByID(tAbelL_layer *tileSet, tAbel_vec2 pos, uint32_t id);

#endif