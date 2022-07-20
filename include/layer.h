#ifndef ABEL_LAYER_H
#define ABEL_LAYER_H

#include "abel.h"
#include "core/hashmap.h"
#include "vec2.h"

#define TILESET_SIZE 16

typedef struct _tAbelL_layer
{
    tAbelR_texture *tileSet;
    tAbelR_texture *tileFrame;
    tAbelR_texture *spriteFrame;
    tAbel_vec2 pos;
} tAbelL_layer;

tAbelL_layer *AbelL_newLayer(tAbelR_texture *tileSet, tAbel_vec2 size);
void AbelL_freeLayer(tAbelL_layer *layer);

void AbelL_renderLayer(tAbelL_layer *layer, SDL_Rect *camera);

void AbelL_addBGTile(tAbelL_layer *layer, tAbel_vec2 pos, uint32_t id);

#endif