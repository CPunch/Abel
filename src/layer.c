#include "layer.h"

#include "core/mem.h"
#include "core/serror.h"
#include "render.h"
#include "vec2.h"

const tAbel_vec2 AbelL_tileSize = AbelV_newVec2(TILESET_SIZE, TILESET_SIZE);

/* =======================================[[ Layer API ]]======================================= */

tAbelL_layer *AbelL_newLayer(tAbelR_texture *tileSet, tAbel_vec2 size)
{
    tAbelL_layer *layer = (tAbelL_layer *)AbelM_malloc(sizeof(tAbelL_layer));
    layer->tileSet = tileSet;
    layer->bgFrame = AbelR_newBlankTexture(AbelV_mulVec2(size, AbelL_tileSize));
    layer->spriteFrame = AbelR_newBlankTexture(AbelV_mulVec2(size, AbelL_tileSize));
    layer->pos = AbelV_newVec2(0, 0);
    return layer;
}

void AbelL_freeLayer(tAbelL_layer *layer)
{
    AbelR_freeTexture(layer->spriteFrame);
    AbelR_freeTexture(layer->bgFrame);
    AbelM_free(layer);
}

/* ========================================[[ Drawing ]]======================================== */

void AbelL_renderLayer(tAbelL_layer *layer, SDL_Rect *camera)
{
    SDL_Rect windowRect = (SDL_Rect){.x = layer->pos.x - AbelR_camera.x,
                                     .y = layer->pos.y - AbelR_camera.y,
                                     .w = AbelR_windowSize.x / 4,
                                     .h = AbelR_windowSize.y / 4};

    /* render bg frame */
    SDL_RenderCopy(AbelR_renderer, layer->bgFrame->texture, &windowRect, NULL);
}

void AbelL_drawTile(tAbelL_layer *layer, tAbel_vec2 pos, uint32_t id, LAYER_FRAME frame)
{
    SDL_Rect src;

    /* get tileset clip */
    src = AbelL_getTileRect(layer, id);

    /* draw to frame */
    AbelL_drawTileClip(layer, src, pos, frame);
}

void AbelL_drawTileClip(tAbelL_layer *layer, SDL_Rect tileClip, tAbel_vec2 pos, LAYER_FRAME frame)
{
    SDL_Rect dest;

    /* get clip of render target */
    dest = (SDL_Rect){.x = pos.x, .y = pos.y, .w = AbelL_tileSize.x, .h = AbelL_tileSize.y};

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

SDL_Rect AbelL_getTileRect(tAbelL_layer *layer, uint32_t id)
{
    tAbel_vec2 cordSize;
    int x, y;

    /* grabs the x/y cords of our tile in our texture */
    cordSize = AbelV_divVec2(layer->tileSet->size, AbelL_tileSize);
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

tAbel_vec2 AbelL_gridToPos(tAbel_vec2 gridPos)
{
    return AbelV_mulVec2(gridPos, AbelL_tileSize);
}