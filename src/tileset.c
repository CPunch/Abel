#include "tileset.h"
#include "render.h"

#include "mem.h"
#include "vec2.h"

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

SDL_Texture *AbelT_getTileByID(tAbelT_tileSet *tileSet, uint32_t id)
{
    
}