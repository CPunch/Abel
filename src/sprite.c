#include "sprite.h"

#include "core/mem.h"
#include "layer.h"

tAbelS_sprite *AbelS_newSprite(tAbelL_layer *layer, TILE_ID id)
{
    tAbelS_sprite *sprite = (tAbelS_sprite *)AbelM_malloc(sizeof(tAbelS_sprite));
    sprite->layer = layer;

    /* setup sprite in layer */
    AbelS_setSpritePos(sprite, AbelV_newVec2(0, 0));
    AbelS_setSpriteTile(sprite, id);
    AbelL_addSprite(layer, sprite);
}

void AbelS_freeSprite(tAbelS_sprite *sprite)
{
    AbelL_rmvSprite(sprite->layer, sprite);
    AbelM_free(sprite);
}

void AbelS_setSpriteTile(tAbelS_sprite *sprite, TILE_ID id)
{
    sprite->clip = AbelL_getTileClip(sprite->layer, id);
}

void AbelS_setSpritePos(tAbelS_sprite *sprite, tAbel_vec2 pos)
{
    sprite->pos = pos;
}

void AbelS_drawSprite(tAbelS_sprite *sprite)
{
    AbelL_drawTileClip(sprite->layer, sprite->clip, sprite->pos, FRAME_SPRITE);
}

tAbelS_sprite *AbelS_getSprite(SPRITE_ID id)
{
    tAbelS_sprite *sprite;

    return sprite;
}