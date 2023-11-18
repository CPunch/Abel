#include "sprite.h"

#include "core/mem.h"
#include "core/serror.h"
#include "core/tasks.h"
#include "render.h"

/* ======================================[[ Animations ]]======================================= */

static tAbelS_animation newAnimation()
{
    tAbelS_animation animation = (tAbelS_animation){
        .animationFrame = -1,
    };
    AbelM_initVector(animation.frames, 2);
    return animation;
}

static void freeAnimation(tAbelS_animation *animation)
{
    AbelM_free(animation->frames);
}

static uint32_t animationTask(uint32_t tick, void *uData)
{
    tAbelS_animationStates *states = (tAbelS_animationStates *)uData;
    tAbelS_animation *animation = &states->animations[states->animationID];

    /* wrap around animation */
    if (++animation->animationFrame >= AbelM_countVector(animation->frames))
        animation->animationFrame = 0;

    /* delay until *next* frame */
    return animation->frames[animation->animationFrame].delay;
}

static int addAnimation(tAbelS_animationStates *states)
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

static void addSpriteFrame(tAbelS_animationStates *states, int animationID, SDL_Rect clip, uint32_t delay)
{
    /* grab current animation */
    tAbelS_animation *animation;

    if (animationID >= AbelM_countVector(states->animations) || animationID < 0)
        ABEL_ERROR("Invalid animation ID: %d\n", animationID);

    animation = &states->animations[animationID];

    /* add animation frame */
    AbelM_growVector(tAbelS_animationData, animation->frames, 1);
    animation->frames[AbelM_countVector(animation->frames)++] = (tAbelS_animationData){.clip = clip, .delay = delay};

    /* set the first frame */
    if (animation->animationFrame == -1)
        animation->animationFrame = 0;
}

static void playAnimation(tAbelS_animationStates *states, int animationID)
{
    /* grab current animation */
    tAbelS_animation *animation;

    if (animationID == states->animationID)
        return; /* already playing animation */

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
    if (states->animationTask == NULL && AbelM_countVector(animation->frames) > 1) {
        /* start the timer at the *next* frame */
        states->animationTask = AbelT_newTask(animationTask(0, (void *)states), animationTask, (void *)states);
    }
}

static void stopAnimation(tAbelS_animationStates *states)
{
    if (states->animationTask != NULL)
        AbelT_freeTask(states->animationTask);
}

static SDL_Rect getCurrentClip(tAbelS_animationStates *states)
{
    tAbelS_animation *animation;

    /* grab current animation */
    if (states->animationID == -1 || (animation = &states->animations[states->animationID])->animationFrame == -1)
        return (SDL_Rect){0}; /* invalid clip */

    /* return current clip */
    return animation->frames[animation->animationFrame].clip;
}

static tAbelS_animationStates newAState()
{
    tAbelS_animationStates states = (tAbelS_animationStates){.animationID = -1, .animationTask = NULL};
    AbelM_initVector(states.animations, 2);
    return states;
}

static void freeAState(tAbelS_animationStates *states)
{
    int i;
    stopAnimation(states);

    /* free animations */
    for (i = 0; i < AbelM_countVector(states->animations); i++)
        freeAnimation(&states->animations[i]);

    AbelM_free(states->animations);
}

/* ======================================[[ Sprite API ]]======================================= */

void AbelS_initSprite(tAbelS_sprite *sprite, tAbelR_texture *tileSet, tAbelV_fVec2 pos)
{
    sprite->animations = newAState();
    sprite->tileSet = tileSet;

    /* setup sprite */
    AbelS_setSpritePos(sprite, pos);
    AbelR_retainTexture(tileSet);
}

void AbelS_cleanupSprite(tAbelS_sprite *sprite)
{
    freeAState(&sprite->animations);
    AbelR_releaseTexture(sprite->tileSet);
}

/* =======================================[[ Setters ]]========================================= */

void AbelS_addFrame(tAbelS_sprite *sprite, int animationID, TILE_ID id, uint32_t delay)
{
    addSpriteFrame(&sprite->animations, animationID, AbelR_getTileClip(sprite->tileSet, id), delay);
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

void AbelS_setSpritePos(tAbelS_sprite *sprite, tAbelV_fVec2 pos)
{
    sprite->pos = pos;
}

void AbelS_drawSprite(tAbelS_sprite *sprite)
{
    tAbelV_iVec2 pos = AbelV_f2iVec(sprite->pos);
    tAbelV_iVec2 offset = AbelR_getCameraOffset();
    tAbelV_iVec2 scale = AbelR_getScale();
    SDL_Rect clip, dest;

    /* get current clip (and check if invalid) */
    clip = getCurrentClip(&sprite->animations);
    if (clip.h == 0 || clip.w == 0)
        return;

    dest = (SDL_Rect){.x = (pos.x * scale.x) + offset.x, .y = (pos.y * scale.y) + offset.y, .w = clip.w * scale.x, .h = clip.h * scale.y};
    SDL_RenderCopy(AbelR_getRenderer(), sprite->tileSet->texture, &clip, &dest);
}