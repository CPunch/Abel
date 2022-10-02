#include "layer.h"

#include "core/mem.h"
#include "core/serror.h"
#include "core/vec2.h"
#include "render.h"
#include "sprite.h"

const tAbel_iVec2 AbelL_tileSize = AbelV_newiVec2(TILESET_SIZE, TILESET_SIZE);

/* =======================================[[ Layer API ]]======================================= */

tAbelL_layer *AbelL_newLayer(tAbelR_texture *tileSet, tAbel_iVec2 size)
{
    tAbelL_layer *layer = (tAbelL_layer *)AbelM_malloc(sizeof(tAbelL_layer));
    layer->tileSet = tileSet;
    layer->bgFrame = AbelR_newBlankTexture(AbelV_muliVec2(size, AbelL_tileSize));
    layer->spriteFrame = AbelR_newBlankTexture(AbelV_muliVec2(size, AbelL_tileSize));
    layer->pos = AbelV_newiVec2(0, 0);
    layer->size = AbelV_muliVec2(size, AbelL_tileSize);
    layer->camera = NULL;
    AbelM_initVector(layer->sprites, 4);
    return layer;
}

void AbelL_freeLayer(tAbelL_layer *layer)
{
    AbelR_freeTexture(layer->spriteFrame);
    AbelR_freeTexture(layer->bgFrame);
    AbelM_free(layer->sprites);
    AbelM_free(layer);
}

/* ========================================[[ Sprites ]]======================================== */

void AbelL_addSprite(tAbelL_layer *layer, tAbelS_sprite *sprite)
{
    AbelM_growVector(tAbelS_sprite *, layer->sprites, 1);
    layer->sprites[AbelM_countVector(layer->sprites)++] = sprite;
}

void AbelL_rmvSprite(tAbelL_layer *layer, tAbelS_sprite *sprite)
{
    int i;

    /* search for sprite in layer */
    for (i = 0; i < AbelM_countVector(layer->sprites); i++) {
        /* found sprite, remove it */
        if (layer->sprites[i] == sprite) {
            AbelM_rmvVector(layer->sprites, i, 1);
            break;
        }
    }
}

/* ========================================[[ Drawing ]]======================================== */

void AbelL_renderLayer(tAbelL_layer *layer)
{

    SDL_Rect viewPort = (SDL_Rect){.x = layer->pos.x + AbelR_camera.pos.x - (AbelR_camera.size.x / 8),
                                     .y = layer->pos.y + AbelR_camera.pos.y - (AbelR_camera.size.y / 8),
                                     .w = AbelR_camera.size.x / 4,
                                     .h = AbelR_camera.size.y / 4};
    SDL_Rect dest;
    int i, tmp;

    dest = (SDL_Rect){.x = 0, .y = 0, .w = AbelR_camera.size.x, .h = AbelR_camera.size.y};
    if ((tmp = (viewPort.x + viewPort.w) - (layer->size.x + layer->pos.x)) > 0) {
        dest.w -= tmp;
        dest.x += tmp;
    }

    if ((tmp = (viewPort.y + viewPort.y) - (layer->size.y + layer->pos.y)) > 0) {
        dest.h -= tmp;
        dest.y += tmp;
    }

    /* render bg frame */
    SDL_RenderCopy(AbelR_renderer, layer->bgFrame->texture, &viewPort, &dest);

    /* clear sprite frame */
    SDL_SetRenderTarget(AbelR_renderer, layer->spriteFrame->texture);
    SDL_RenderClear(AbelR_renderer);

    /* render each sprite */
    for (i = 0; i < AbelM_countVector(layer->sprites); i++) {
        AbelS_drawSprite(layer->sprites[i]);
    }

    SDL_SetRenderTarget(AbelR_renderer, NULL);
    SDL_RenderCopy(AbelR_renderer, layer->spriteFrame->texture, &viewPort, &dest);
}

void AbelL_drawTile(tAbelL_layer *layer, tAbel_iVec2 pos, TILE_ID id, LAYER_FRAME frame)
{
    SDL_Rect src;

    /* get tileset clip */
    src = AbelL_getTileClip(layer, id);

    /* draw to frame */
    AbelL_drawTileClip(layer, src, pos, frame);
}

void AbelL_drawTileClip(tAbelL_layer *layer, SDL_Rect tileClip, tAbel_iVec2 pos, LAYER_FRAME frame)
{
    SDL_Rect dest;

    /* get clip of render target */
    dest = (SDL_Rect){.x = pos.x, .y = pos.y, .w = AbelL_tileSize.x, .h = AbelL_tileSize.y};

    if (layer->camera) {
        /* apply camera offset */
        dest.x -= layer->camera->x;
        dest.y -= layer->camera->y;

        /* make sure we're within render distance */
        if (dest.x > layer->camera->w  ||
            (dest.x + dest.w) < 0      ||
            dest.y > layer->camera->h  ||
            (dest.y + dest.h) < 0) {
            return;
        }
    }

    /* render to selected frame */
    switch (frame) {
    case FRAME_BG:
        SDL_SetRenderTarget(AbelR_renderer, layer->bgFrame->texture);
        break;
    case FRAME_SPRITE:
        SDL_SetRenderTarget(AbelR_renderer, layer->spriteFrame->texture);
        break;
    default:
        ABEL_ERROR("Invalid frame type passed: %d!\n", frame);
    }

    /* draw clipped texture */
    if (SDL_RenderCopy(AbelR_renderer, layer->tileSet->texture, &tileClip, &dest) != 0)
        ABEL_ERROR("Failed to render tile to target: %s\n", SDL_GetError());
    SDL_SetRenderTarget(AbelR_renderer, NULL);
}

/* =========================================[[ Utils ]]========================================= */

void AbelL_setCamera(tAbelL_layer *layer, SDL_Rect *camera)
{
    layer->camera = camera;
}

SDL_Rect AbelL_getTileClip(tAbelL_layer *layer, TILE_ID id)
{
    tAbel_iVec2 cordSize;
    int x, y;

    /* grabs the x/y cords of our tile in our texture */
    cordSize = AbelV_diviVec2(layer->tileSet->size, AbelL_tileSize);
    y = id / cordSize.x;
    x = id % cordSize.x;

    if (y > cordSize.y)
        ABEL_ERROR("Invalid tile id: %d\n", id);

    /* return clip of texture */
    return (SDL_Rect){.x = x * AbelL_tileSize.x,
                      .y = y * AbelL_tileSize.y,
                      .w = AbelL_tileSize.x,
                      .h = AbelL_tileSize.y};
}

tAbel_iVec2 AbelL_gridToPos(tAbel_iVec2 gridPos)
{
    return AbelV_muliVec2(gridPos, AbelL_tileSize);
}

tAbel_iVec2 AbelL_posToGrid(tAbel_iVec2 pos)
{
    return AbelV_diviVec2(pos, AbelL_tileSize);
}