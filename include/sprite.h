#ifndef ABEL_SPRITE_H
#define ABEL_SPRITE_H

#include "abel.h"
#include "vec2.h"

typedef struct _tAbelS_sprite
{
    SDL_Rect clip;
    tAbel_vec2 pos;
    tAbelL_layer *layer;
    SPRITE_ID id;
} tAbelS_sprite;

tAbelS_sprite *AbelS_newSprite(tAbelL_layer *layer, TILE_ID id);
void AbelS_freeSprite(tAbelS_sprite *sprite);

void AbelS_setSpriteTile(tAbelS_sprite *sprite, TILE_ID id);
void AbelS_setSpritePos(tAbelS_sprite *sprite, tAbel_vec2 pos);

tAbelS_sprite *AbelS_getSprite(SPRITE_ID id);

void AbelS_drawSprite(tAbelS_sprite *sprite);

#endif