#ifndef ABEL_LAYER_H
#define ABEL_LAYER_H

#include "abel.h"
#include "core/hashmap.h"
#include "core/mem.h"
#include "core/vec2.h"

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
    AbelM_newVector(tAbelS_sprite *, sprites);
    tAbel_iVec2 pos;
    tAbel_iVec2 size;
    SDL_Rect *camera; /* NULL = don't follow camera */
} tAbelL_layer;

/* layers */
tAbelL_layer *AbelL_newLayer(tAbelR_texture *tileSet, tAbel_iVec2 size);
void AbelL_freeLayer(tAbelL_layer *layer);

/* sprites */
void AbelL_addSprite(tAbelL_layer *layer, tAbelS_sprite *sprite);
void AbelL_rmvSprite(tAbelL_layer *layer, tAbelS_sprite *sprite);

/* drawing */
void AbelL_renderLayer(tAbelL_layer *layer);
void AbelL_drawTile(tAbelL_layer *layer, tAbel_iVec2 pos, TILE_ID id, LAYER_FRAME frame);
void AbelL_drawTileClip(tAbelL_layer *layer, SDL_Rect tileClip, tAbel_iVec2 pos, LAYER_FRAME frame);

/* utils */
void AbelL_setCamera(tAbelL_layer *layer, SDL_Rect *camera);
SDL_Rect AbelL_getTileClip(tAbelL_layer *layer, TILE_ID id);
tAbel_iVec2 AbelL_gridToPos(tAbel_iVec2 gridPos);
tAbel_iVec2 AbelL_posToGrid(tAbel_iVec2 pos);

#endif