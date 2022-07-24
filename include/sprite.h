#ifndef ABEL_SPRITE_H
#define ABEL_SPRITE_H

#include "abel.h"
#include "core/mem.h"
#include "vec2.h"

typedef struct _tAbelS_animationData
{
    SDL_Rect clip;
    uint32_t delay; /* time in ms for current frame */
} tAbelS_animationData;

typedef struct _tAbelS_sprite
{
    tAbel_vec2 pos;
    tAbelL_layer *layer;
    AbelM_newVector(tAbelS_animationData, animation);
    int animationID;
    SDL_TimerID animationTimer;
} tAbelS_sprite;

tAbelS_sprite *AbelS_newSprite(tAbelL_layer *layer, tAbel_vec2 pos);
void AbelS_freeSprite(tAbelS_sprite *sprite);

void AbelS_addSprite(tAbelS_sprite *sprite, TILE_ID id, uint32_t delay);
void AbelS_setSpritePos(tAbelS_sprite *sprite, tAbel_vec2 pos);

void AbelS_drawSprite(tAbelS_sprite *sprite);

#endif