#ifndef ABEL_SPRITE_H
#define ABEL_SPRITE_H

#include "abel.h"
#include "core/mem.h"
#include "core/vec2.h"

/* animation frame */
typedef struct _tAbelS_animationData
{
    SDL_Rect clip;
    uint32_t delay; /* time in ms for current frame */
} tAbelS_animationData;

typedef struct _tAbelS_animation
{
    AbelM_newVector(tAbelS_animationData, frames);
    int animationFrame; /* current frame */
} tAbelS_animation;

/* animation handler */
typedef struct _tAbelS_animationStates
{
    AbelM_newVector(tAbelS_animation, animations);
    int animationID; /* which animation is playing */
    SDL_TimerID animationTimer;
} tAbelS_animationStates;

typedef struct _tAbelS_sprite
{
    tAbel_fVec2 pos;
    tAbelL_layer *layer;
    tAbelS_animationStates animations;
} tAbelS_sprite;

tAbelS_sprite *AbelS_newSprite(tAbelL_layer *layer, tAbel_fVec2 pos);
void AbelS_freeSprite(tAbelS_sprite *sprite);

void AbelS_addFrame(tAbelS_sprite *sprite, int animationID, TILE_ID id, uint32_t delay);
int AbelS_addAnimation(tAbelS_sprite *sprite);
void AbelS_playAnimation(tAbelS_sprite *sprite, int animationID);
void AbelS_stopAnimation(tAbelS_sprite *sprite);
void AbelS_setSpritePos(tAbelS_sprite *sprite, tAbel_fVec2 pos);

void AbelS_drawSprite(tAbelS_sprite *sprite);

#endif