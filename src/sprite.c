#include "sprite.h"

#include "core/mem.h"
#include "core/serror.h"
#include "layer.h"

/* ======================================[[ Animations ]]======================================= */

tAbelS_animation newAnimation()
{
    tAbelS_animation animation = (tAbelS_animation){
        .animationFrame = -1,
    };
    AbelM_initVector(animation.frames, 2);
    return animation;
}

void freeAnimation(tAbelS_animation *animation)
{
    AbelM_free(animation->frames);
}

uint32_t animationTimer(uint32_t tick, void *uData)
{
    tAbelS_animationStates *states = (tAbelS_animationStates *)uData;
    /* grab current animation */
    tAbelS_animation *animation = &states->animations[states->animationID];

    /* wrap around animation */
    if (++animation->animationFrame >= AbelM_countVector(animation->frames))
        animation->animationFrame = 0;

    /* delay until *next* frame */
    return animation->frames[animation->animationFrame].delay;
}

int addAnimation(tAbelS_animationStates *states)
{
    tAbelS_animation animation = newAnimation();
    int animationID = AbelM_countVector(states->animations)++;

    /* add animation */
    AbelM_growVector(tAbelS_animation, states->animations, 1);
    states->animations[animationID] = animation;

    /* set the first animation */
    if (states->animationID == -1)
        states->animationID = animationID;

    return animationID;
}

void addSpriteFrame(tAbelS_animationStates *states, int animationID, SDL_Rect clip, uint32_t delay)
{
    /* grab current animation */
    tAbelS_animation *animation;

    if (animationID >= AbelM_countVector(states->animations) || animationID < 0)
        ABEL_ERROR("Invalid animation ID: %d\n", animationID);

    animation = &states->animations[animationID];

    /* add animation frame */
    AbelM_growVector(tAbelS_animationData, animation->frames, 1);
    animation->frames[AbelM_countVector(animation->frames)++] =
        (tAbelS_animationData){.clip = clip, .delay = delay};

    /* set the first frame */
    if (animation->animationFrame == -1)
        animation->animationFrame = 0;
}

void playAnimation(tAbelS_animationStates *states, int animationID)
{
    /* grab current animation */
    tAbelS_animation *animation;

    if (animationID >= AbelM_countVector(states->animations) || animationID < 0)
        ABEL_ERROR("Invalid animation ID: %d\n", animationID);

    animation = &states->animations[animationID];

    /* make sure we have frames in our animation */
    if (AbelM_countVector(animation->frames) == 0)
        ABEL_ERROR("Cannot play empty animation!\n");

    /* set animation state */
    states->animationID = animationID;
    animation->animationFrame = 0;

     /* if the timer is NOT running, start our timer (if we have more than 1 frame) */
    if (states->animationTimer == 0 && AbelM_countVector(animation->frames) > 1) {
        /* start the timer at the *next* frame */
        states->animationTimer = SDL_AddTimer(animationTimer(SDL_GetTicks(), (void *)states),
                                              animationTimer, (void *)states);
    }
}

void stopAnimation(tAbelS_animationStates *states)
{
    if (states->animationTimer != 0)
        SDL_RemoveTimer(states->animationTimer);
}

SDL_Rect getCurrentClip(tAbelS_animationStates *states)
{
    tAbelS_animation *animation;

    /* grab current animation */
    if (states->animationID == -1 || (animation = &states->animations[states->animationID])->animationFrame == -1)
        return (SDL_Rect){0}; /* invalid clip */

    /* return current clip */
    return animation->frames[animation->animationFrame].clip;
}

tAbelS_animationStates newAState()
{
    tAbelS_animationStates states = (tAbelS_animationStates){
        .animationID = -1,
        /* 0 is a reserved timerID error result, so it should be safe to use as an invalid ID */
        .animationTimer = 0
    };
    AbelM_initVector(states.animations, 2);
    return states;
}

void freeAState(tAbelS_animationStates *states)
{
    int i;
    stopAnimation(states);

    /* free animations */
    for (i = 0; i < AbelM_countVector(states->animations); i++)
        freeAnimation(&states->animations[i]);

    AbelM_free(states->animations);
}

/* ======================================[[ Sprite API ]]======================================= */

tAbelS_sprite *AbelS_newSprite(tAbelL_layer *layer, tAbel_vec2 pos)
{
    tAbelS_sprite *sprite = (tAbelS_sprite *)AbelM_malloc(sizeof(tAbelS_sprite));
    sprite->layer = layer;
    sprite->animations = newAState();

    /* setup sprite in layer */
    AbelS_setSpritePos(sprite, pos);
    AbelL_addSprite(layer, sprite);
}

void AbelS_freeSprite(tAbelS_sprite *sprite)
{
    freeAState(&sprite->animations);
    AbelL_rmvSprite(sprite->layer, sprite);
    AbelM_free(sprite);
}

/* =======================================[[ Setters ]]========================================= */

void AbelS_addFrame(tAbelS_sprite *sprite, int animationID, TILE_ID id, uint32_t delay)
{
    addSpriteFrame(&sprite->animations, animationID, AbelL_getTileClip(sprite->layer, id), delay);
}

int AbelS_addAnimation(tAbelS_sprite *sprite)
{
    return addAnimation(&sprite->animations);
}

void AbelS_playAnimation(tAbelS_sprite *sprite, int animationID)
{
    playAnimation(&sprite->animations, animationID);
}

void AbelS_stopAnimation(tAbelS_sprite *sprite)
{
    stopAnimation(&sprite->animations);
}

void AbelS_setSpritePos(tAbelS_sprite *sprite, tAbel_vec2 pos)
{
    sprite->pos = pos;
}

void AbelS_drawSprite(tAbelS_sprite *sprite)
{
    /* get current clip (and check if invalid) */
    SDL_Rect clip = getCurrentClip(&sprite->animations);
    if (clip.h == 0 || clip.w == 0)
        return;

    AbelL_drawTileClip(sprite->layer, clip, sprite->pos, FRAME_SPRITE);
}