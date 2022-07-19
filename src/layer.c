#include "layer.h"

#include "core/mem.h"
#include "core/serror.h"
#include "render.h"
#include "vec2.h"

/* ======================================[[ TileSet API ]]====================================== */

tAbelL_layer *AbelL_newLayer(tAbelR_texture *tileSet, tAbel_vec2 size)
{
    SDL_Texture *frame;
    tAbelL_layer *layer;

    /* create bg frame */
    frame = SDL_CreateTexture(AbelR_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                              size.x * TILESET_SIZE, size.y * TILESET_SIZE);
    if (frame == NULL)
        ABEL_ERROR("Failed to make blank texture: %s\n", SDL_GetError());

    /* make layer */
    layer = (tAbelL_layer *)AbelM_malloc(sizeof(tAbelL_layer));
    layer->tileSet = tileSet;
    layer->frame = AbelR_newTexture(frame);
    return layer;
}

void AbelL_freeLayer(tAbelL_layer *tileSet)
{
    AbelR_freeTexture(tileSet->tileSet);
    AbelM_free(tileSet);
}

void AbelL_renderTileByID(tAbelL_layer *layer, tAbel_vec2 pos, uint32_t id)
{
    const tAbel_vec2 tileSize = AbelV_newVec2(TILESET_SIZE, TILESET_SIZE);
    SDL_Rect src, dest;
    tAbel_vec2 cordSize;
    int x, y;

    /* grabs the x/y cords of our tile in our texture */
    cordSize = AbelV_divVec2(layer->tileSet->size, tileSize);
    y = id / cordSize.x;
    x = id % cordSize.x;

    if (y > cordSize.y)
        ABEL_ERROR("Invalid tile id: %d\n", id);

    /* get clip of texture */
    src = (SDL_Rect){.x = x * tileSize.x, .y = y * tileSize.y, .w = tileSize.x, .h = tileSize.y};

    /* get clip of render target */
    dest = (SDL_Rect){
        .x = pos.x * tileSize.x, .y = pos.y * tileSize.y, .w = tileSize.x, .h = tileSize.y};

    /* render to target */
    SDL_SetRenderTarget(AbelR_renderer, layer->frame->texture);
    if (SDL_RenderCopy(AbelR_renderer, layer->tileSet->texture, &src, &dest) != 0)
        ABEL_ERROR("Failed to render tile to target: %s\n", SDL_GetError());
    SDL_SetRenderTarget(AbelR_renderer, NULL);
}