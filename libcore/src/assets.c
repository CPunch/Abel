#include "assets.h"

#include "core/mem.h"
#include "core/serror.h"
#include "core/vec2.h"
#include "render.h"
#include "sound.h"

void AbelA_init(uint32_t initFlags)
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

tAbelR_sound *AbelA_getSound(const char *path)
{
    Mix_Chunk *chunk = Mix_LoadWAV(path);
    if (chunk == NULL)
        ABEL_ERROR("Failed to load sound from '%s': %s\n", path, Mix_GetError());

    return AbelR_newSound(chunk);
}
