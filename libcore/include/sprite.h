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
    tAbelT_task *animationTask;
    int multiplier;
} tAbelS_animationStates;

typedef struct _tAbelS_sprite
{
    tAbelS_animationStates animations;
    tAbelV_fVec2 pos;
    tAbelR_texture *tileSet;
} tAbelS_sprite;

void AbelS_initSprite(tAbelS_sprite *sprite, tAbelR_texture *tileSet, tAbelV_fVec2 pos);
void AbelS_cleanupSprite(tAbelS_sprite *sprite);

void AbelS_addFrame(tAbelS_sprite *sprite, int animationID, TILE_ID id, uint32_t delay);
int AbelS_addAnimation(tAbelS_sprite *sprite);
void AbelS_playAnimation(tAbelS_sprite *sprite, int animationID);
void AbelS_stopAnimation(tAbelS_sprite *sprite);
void AbelS_speedUpAnimation(tAbelS_sprite *sprite, int multiplier);
void AbelS_setSpritePos(tAbelS_sprite *sprite, tAbelV_fVec2 pos);

void AbelS_drawSprite(tAbelS_sprite *sprite);

#endif