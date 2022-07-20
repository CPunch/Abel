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
    layer->tileFrame = AbelR_newBlankTexture(AbelV_mulVec2(size, AbelL_tileSize));
    layer->spriteFrame = AbelR_newBlankTexture(AbelV_mulVec2(size, AbelL_tileSize));
    layer->pos = AbelV_newVec2(0, 0);
    return layer;
}

void AbelL_freeLayer(tAbelL_layer *layer)
{
    AbelR_freeTexture(layer->tileSet);
    AbelM_free(layer);
}

void AbelL_renderLayer(tAbelL_layer *layer, SDL_Rect *camera)
{
    SDL_Rect windowRect = (SDL_Rect){.x = layer->pos.x - AbelR_camera.x,
                                     .y = layer->pos.y - AbelR_camera.y,
                                     .w = AbelR_windowSize.x / 4,
                                     .h = AbelR_windowSize.y / 4};

    /* render tile frame */
    SDL_RenderCopy(AbelR_renderer, layer->tileFrame->texture, &windowRect, NULL);
}

void AbelL_addBGTile(tAbelL_layer *layer, tAbel_vec2 pos, uint32_t id)
{
    SDL_Rect src, dest;
    tAbel_vec2 cordSize;
    int x, y;

    /* grabs the x/y cords of our tile in our texture */
    cordSize = AbelV_divVec2(layer->tileSet->size, AbelL_tileSize);
    y = id / cordSize.x;
    x = id % cordSize.x;

    if (y > cordSize.y)
        ABEL_ERROR("Invalid tile id: %d\n", id);

    /* get clip of texture */
    src = (SDL_Rect){.x = x * AbelL_tileSize.x,
                     .y = y * AbelL_tileSize.y,
                     .w = AbelL_tileSize.x,
                     .h = AbelL_tileSize.y};

    /* get clip of render target */
    dest = (SDL_Rect){.x = pos.x * AbelL_tileSize.x,
                      .y = pos.y * AbelL_tileSize.y,
                      .w = AbelL_tileSize.x,
                      .h = AbelL_tileSize.y};

    /* render to target */
    SDL_SetRenderTarget(AbelR_renderer, layer->tileFrame->texture);
    if (SDL_RenderCopy(AbelR_renderer, layer->tileSet->texture, &src, &dest) != 0)
        ABEL_ERROR("Failed to render tile to target: %s\n", SDL_GetError());
    SDL_SetRenderTarget(AbelR_renderer, NULL);
}