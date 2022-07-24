#include "sprite.h"

#include "core/mem.h"
#include "layer.h"

/* ===================================[[ Helper Functions ]]==================================== */

uint32_t animationTimer(uint32_t tick, void *uData)
{
    tAbelS_sprite *sprite = (tAbelS_sprite *)uData;

    /* wrap around animation */
    if (++sprite->animationID >= sprite->animation_COUNT)
        sprite->animationID = 0;

    /* delay until *next* frame */
    return sprite->animation[sprite->animationID].delay;
}

/* ======================================[[ Sprite API ]]======================================= */

tAbelS_sprite *AbelS_newSprite(tAbelL_layer *layer, tAbel_vec2 pos)
{
    tAbelS_sprite *sprite = (tAbelS_sprite *)AbelM_malloc(sizeof(tAbelS_sprite));
    sprite->layer = layer;
    sprite->animationID = -1;
    sprite->animationTimer = false;
    AbelM_initVector(sprite->animation, 2);

    /* setup sprite in layer */
    AbelS_setSpritePos(sprite, pos);
    AbelL_addSprite(layer, sprite);
}

void AbelS_freeSprite(tAbelS_sprite *sprite)
{
    AbelL_rmvSprite(sprite->layer, sprite);
    AbelM_free(sprite);
}

void AbelS_addSprite(tAbelS_sprite *sprite, TILE_ID id, uint32_t delay)
{
    AbelM_growVector(tAbelS_animationData, sprite->animation, 1);
    sprite->animation[sprite->animation_COUNT++] =
        (tAbelS_animationData){.clip = AbelL_getTileClip(sprite->layer, id), .delay = delay};

    /* if the timer is NOT running, start our timer */
    if (!sprite->animationTimer) {
        sprite->animationTimer = true;

        /* start the timer at the *next* frame */
        SDL_AddTimer(animationTimer(SDL_GetTicks(), (void *)sprite), animationTimer,
                     (void *)sprite);
    }
}

void AbelS_setSpritePos(tAbelS_sprite *sprite, tAbel_vec2 pos)
{
    sprite->pos = pos;
}

void AbelS_drawSprite(tAbelS_sprite *sprite)
{
    /* no sprite is set */
    if (sprite->animationID == -1)
        return;

    AbelL_drawTileClip(sprite->layer, sprite->animation[sprite->animationID].clip, sprite->pos,
                       FRAME_SPRITE);
}

tAbelS_sprite *AbelS_getSprite(SPRITE_ID id)
{
    tAbelS_sprite *sprite;

    return sprite;
}