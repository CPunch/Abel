#include "render.h"

#include "core/mem.h"
#include "core/serror.h"

SDL_Window *AbelR_window = NULL;
SDL_Renderer *AbelR_renderer = NULL;
tAbel_iVec2 AbelR_windowSize;
tAbel_iVec2 AbelR_camera;

#define SDL_IMG_FLAGS IMG_INIT_PNG

/* =====================================[[ Initializers ]]====================================== */

void AbelR_init(void)
{
    /* pretty sure sdl_img & ttf_img are initalized with this,,, but i think the API is for
        previous versions of SDL so... we'll go ahead and do it anyways */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
        ABEL_ERROR("Failed to initialize SDL: %s\n", SDL_GetError());

    /* IMG_Init() returns the flags set, make sure they match */
    if (IMG_Init(SDL_IMG_FLAGS) != SDL_IMG_FLAGS)
        ABEL_ERROR("Failed to initialize SDL_IMG: %s\n", IMG_GetError());

    if (TTF_Init() != 0)
        ABEL_ERROR("Failed to initialize: SDL_TTF: %s\n", TTF_GetError());

    /* open window */
    AbelR_windowSize = AbelV_newiVec2(START_SCREEN_WIDTH, START_SCREEN_HEIGHT);
    AbelR_window = SDL_CreateWindow("Abel", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    AbelR_windowSize.x, AbelR_windowSize.y, SDL_WINDOW_SHOWN);
    if (AbelR_window == NULL)
        ABEL_ERROR("Failed to open window: %s\n", SDL_GetError());

    /* create & set rendering target */
    AbelR_renderer =
        SDL_CreateRenderer(AbelR_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (AbelR_renderer == NULL)
        ABEL_ERROR("Failed to create renderer target: %s\n", SDL_GetError());

    /* init camera */
    AbelR_camera = AbelV_newiVec2(0, 0);

    SDL_SetRenderTarget(AbelR_renderer, NULL);
}

void AbelR_quit(void)
{
    SDL_DestroyRenderer(AbelR_renderer);
    SDL_DestroyWindow(AbelR_window);
    TTF_Quit();
    IMG_Quit();
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

    /* make sure we can render textures *on top of* others, keep transparency */
    SDL_SetTextureBlendMode(rawTexture, SDL_BLENDMODE_BLEND);
    return texture;
}

void AbelR_freeTexture(tAbelR_texture *texture)
{
    SDL_DestroyTexture(texture->texture);
    AbelM_free(texture);
}

tAbelR_texture *AbelR_newBlankTexture(tAbel_iVec2 size)
{
    SDL_Texture *rawTexture;

    /* create SDL texture */
    rawTexture = SDL_CreateTexture(AbelR_renderer, SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_TARGET, size.x, size.y);
    if (rawTexture == NULL)
        ABEL_ERROR("Failed to make blank texture: %s\n", SDL_GetError());

    /* create Abel texture */
    return AbelR_newTexture(rawTexture);
}