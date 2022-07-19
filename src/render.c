#include "render.h"
#include "serror.h"
#include "mem.h"

SDL_Window *AbelR_window = NULL;
SDL_Renderer *AbelR_renderer = NULL;

void AbelR_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        ABEL_ERROR("Failed to initialize SDL: %s\n", SDL_GetError());

    /* open window */
    AbelR_window = SDL_CreateWindow("Abel",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800, 200,
        SDL_WINDOW_SHOWN
    );
    if (AbelR_window == NULL)
        ABEL_ERROR("Failed to open window: %s\n", SDL_GetError());

    /* create & set rendering target */
    AbelR_renderer =  SDL_CreateRenderer(AbelR_window, -1, SDL_RENDERER_ACCELERATED);
    if (AbelR_renderer == NULL)
        ABEL_ERROR("Failed to create renderer target: %s\n", SDL_GetError());

    SDL_SetRenderTarget(AbelR_renderer, NULL);
}

void AbelR_quit(void)
{
    SDL_DestroyRenderer(AbelR_renderer);
    SDL_DestroyWindow(AbelR_window);
    SDL_Quit();
}

/* ======================================[[ Texture API ]]====================================== */

tAbelR_texture *AbelR_newTexture(SDL_Texture *rawTexture)
{
    tAbelR_texture *texture = (tAbelR_texture *)AbelM_malloc(sizeof(tAbelR_texture));
    texture->texture = rawTexture;

    /* grab texture size */
    if (SDL_QueryTexture(rawTexture, NULL, NULL, &texture->size.x, &texture->size.y) != 0) 
        ABEL_ERROR("Failed to query texture information: %s\n", SDL_GetError());

    return texture;
}

void AbelR_freeTexture(tAbelR_texture *texture)
{
    SDL_DestroyTexture(texture->texture);
    AbelM_free(texture);
}