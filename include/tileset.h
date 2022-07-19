#ifndef ABEL_TILESET_H
#define ABEL_TILESET_H

#include "abel.h"
#include "vec2.h"

typedef struct _tAbelT_tileSet {
    tAbelR_texture *texture;
    tAbel_vec2 tileSize;
} tAbelT_tileSet;

tAbelT_tileSet *AbelT_newTileSet(tAbelR_texture *texture, tAbel_vec2 tileSize);
void AbelT_freeTileSet(tAbelT_tileSet *tileSet);

SDL_Texture *AbelT_getTileByID(tAbelT_tileSet *tileSet, uint32_t id);

#endif