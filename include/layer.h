#ifndef ABEL_LAYER_H
#define ABEL_LAYER_H

#include "abel.h"
#include "core/hashmap.h"
#include "vec2.h"

#define TILESET_SIZE 16

typedef enum
{
    FRAME_BG,    /* static frame, never cleared */
    FRAME_SPRITE /* cleared & redrawn every frame */
} LAYER_FRAME;

typedef struct _tAbelL_layer
{
    tAbelR_texture *tileSet;
    tAbelR_texture *bgFrame;
    tAbelR_texture *spriteFrame;
    tAbel_vec2 pos;
} tAbelL_layer;

tAbelL_layer *AbelL_newLayer(tAbelR_texture *tileSet, tAbel_vec2 size);
void AbelL_freeLayer(tAbelL_layer *layer);

void AbelL_renderLayer(tAbelL_layer *layer, SDL_Rect *camera);

void AbelL_drawTile(tAbelL_layer *layer, tAbel_vec2 pos, uint32_t id, LAYER_FRAME frame);
void AbelL_drawTileClip(tAbelL_layer *layer, SDL_Rect tileClip, tAbel_vec2 pos, LAYER_FRAME frame);

SDL_Rect AbelL_getTileRect(tAbelL_layer *layer, uint32_t id);
tAbel_vec2 AbelL_gridToPos(tAbel_vec2 gridPos);

#endif