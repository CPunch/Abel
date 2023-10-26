#include "render.h"

#include "chunk.h"
#include "core/mem.h"
#include "core/serror.h"

tAbelV_iVec2 AbelR_tileSize = AbelV_newiVec2(TILESET_SIZE, TILESET_SIZE);

#define SDL_IMG_FLAGS IMG_INIT_PNG

typedef struct _tAbelR_State
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    tAbelR_camera camera;
} tAbelR_state;

static tAbelR_state AbelR_state;

static void openWindow(int width, int height)
{
    /* init camera */
    AbelR_state.camera.pos = AbelV_newiVec2(0, 0);
    AbelR_state.camera.size = AbelV_newiVec2(width, height);

    /* open window */
    AbelR_state.window = SDL_CreateWindow("Abel", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (AbelR_state.window == NULL)
        ABEL_ERROR("Failed to open window: %s\n", SDL_GetError());

    /* create & set rendering target */
    AbelR_state.renderer = SDL_CreateRenderer(AbelR_state.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (AbelR_state.renderer == NULL)
        ABEL_ERROR("Failed to create renderer target: %s\n", SDL_GetError());

    SDL_SetRenderTarget(AbelR_state.renderer, NULL);
}

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

    openWindow(START_SCREEN_WIDTH, START_SCREEN_HEIGHT);
}

void AbelR_quit(void)
{
    SDL_DestroyRenderer(AbelR_state.renderer);
    SDL_DestroyWindow(AbelR_state.window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

SDL_Renderer *AbelR_getRenderer(void)
{
    return AbelR_state.renderer;
}

tAbelR_camera *AbelR_getCamera(void)
{
    return &AbelR_state.camera;
}

/* ======================================[[ Texture API ]]====================================== */

tAbelR_texture *AbelR_newTexture(SDL_Texture *rawTexture)
{
    tAbelR_texture *texture = (tAbelR_texture *)AbelM_malloc(sizeof(tAbelR_texture));
    texture->texture = rawTexture;

    /* grab texture size */
    if (SDL_QueryTexture(rawTexture, NULL, NULL, (int *)&texture->size.x, (int *)&texture->size.y) != 0)
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

tAbelR_texture *AbelR_newBlankTexture(tAbelV_iVec2 size)
{
    SDL_Texture *rawTexture;

    /* create SDL texture */
    rawTexture = SDL_CreateTexture(AbelR_state.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.x, size.y);
    if (rawTexture == NULL)
        ABEL_ERROR("Failed to make blank texture: %s\n", SDL_GetError());

    /* create Abel texture */
    return AbelR_newTexture(rawTexture);
}

SDL_Rect AbelR_getTileClip(tAbelR_texture *tileSet, TILE_ID id)
{
    tAbelV_iVec2 cordSize;
    int x, y;

    /* grabs the x/y cords of our tile in our texture */
    cordSize = AbelV_diviVec2(tileSet->size, AbelR_tileSize);
    y = id / cordSize.x;
    x = id % cordSize.x;

    if (y > cordSize.y)
        ABEL_ERROR("Invalid tile id: %d\n", id);

    /* return clip of texture */
    return (SDL_Rect){.x = x * AbelR_tileSize.x, .y = y * AbelR_tileSize.y, .w = AbelR_tileSize.x, .h = AbelR_tileSize.y};
}

void AbelR_drawClip(tAbelR_texture *texture, SDL_Rect clip, tAbelV_iVec2 pos)
{
    SDL_Rect dest;

    /* get destination rect */
    dest = (SDL_Rect){.x = pos.x, .y = pos.y, .w = clip.w, .h = clip.h};

    /* render */
    SDL_RenderCopy(AbelR_state.renderer, texture->texture, &clip, &dest);
}