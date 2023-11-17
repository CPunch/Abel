#include "assets.h"

#include "core/mem.h"
#include "core/serror.h"
#include "core/vec2.h"
#include "render.h"

void AbelA_init(void)
{
}

void AbelA_quit(void)
{
}

tAbelR_texture *AbelA_getTexture(const char *path, tAbelV_iVec2 tileSize)
{
    SDL_Texture *rawText;

    /* load raw texture */
    rawText = IMG_LoadTexture(AbelR_getRenderer(), path);
    if (rawText == NULL)
        ABEL_ERROR("Failed to load texture from '%s': %s\n", path, SDL_GetError());

    return AbelR_newTexture(rawText, tileSize);
}
