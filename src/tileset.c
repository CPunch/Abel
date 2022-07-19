#include "tileset.h"

#include "mem.h"
#include "render.h"
#include "serror.h"
#include "vec2.h"

/* ======================================[[ TileSet API ]]====================================== */

tAbelT_tileSet *AbelT_newTileSet(tAbelR_texture *texture, tAbel_vec2 tileSize)
{
    tAbelT_tileSet *tileSet = (tAbelT_tileSet *)AbelM_malloc(sizeof(tAbelT_tileSet));
    tileSet->texture = texture;
    tileSet->tileSize = tileSize;
    return tileSet;
}

void AbelT_freeTileSet(tAbelT_tileSet *tileSet)
{
    AbelR_freeTexture(tileSet->texture);
    AbelM_free(tileSet);
}

void AbelT_renderTileByID(tAbelT_tileSet *tileSet, SDL_Renderer *renderer, tAbel_vec2 pos,
                          uint32_t id)
{
    SDL_Rect src, dest;
    tAbel_vec2 cordSize;
    int x, y;

    /* grabs the x/y cords of our tile in our texture */
    cordSize = AbelV_divVec2(tileSet->texture->size, tileSet->tileSize);
    y = id / cordSize.x;
    x = id % cordSize.x;

    if (y > cordSize.y)
        ABEL_ERROR("Invalid tile id: %d\n", id);

    /* get clip of texture */
    src = (SDL_Rect){.x = x * tileSet->tileSize.x,
                     .y = y * tileSet->tileSize.y,
                     .w = tileSet->tileSize.x,
                     .h = tileSet->tileSize.y};

    /* get clip of render target */
    dest = (SDL_Rect){.x = pos.x * tileSet->tileSize.x,
                      .y = pos.y * tileSet->tileSize.y,
                      .w = tileSet->tileSize.x,
                      .h = tileSet->tileSize.y};

    /* render to target */
    if (SDL_RenderCopy(renderer, tileSet->texture->texture, &src, &dest) != 0)
        ABEL_ERROR("Failed to render tile to target: %s\n", SDL_GetError());
}