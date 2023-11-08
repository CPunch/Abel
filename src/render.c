#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "render.h"

#include "chunk.h"
#include "core/mem.h"
#include "core/serror.h"
#include "core/vec2.h"

tAbelV_iVec2 AbelR_tileSize = AbelV_newiVec2(TILESET_SIZE, TILESET_SIZE);

#define SDL_IMG_FLAGS IMG_INIT_PNG

typedef struct _tAbelR_State
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    struct nk_context *nkCtx;
    tAbelR_camera camera;
    tAbelV_iVec2 scale;
} tAbelR_state;

static tAbelR_state AbelR_state = {0};

static void openWindow(int width, int height)
{
    AbelR_setScale(AbelV_newiVec2(2, 2));

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

    /* set 60 fps cap */
    
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

    /* setup nuklear ui */
    AbelR_state.nkCtx = nk_sdl_init(AbelR_state.window, AbelR_state.renderer);

    {
        struct nk_font_atlas *atlas;
        struct nk_font_config config = nk_font_config(0);
        struct nk_font *font;

        nk_sdl_font_stash_begin(&atlas);
        font = nk_font_atlas_add_from_file(atlas, "res/kongtext.ttf", 14, &config);
        nk_sdl_font_stash_end();

        nk_style_set_font(AbelR_state.nkCtx, &font->handle);
    }
}

void AbelR_quit(void)
{
    nk_sdl_shutdown();
    SDL_DestroyRenderer(AbelR_state.renderer);
    SDL_DestroyWindow(AbelR_state.window);

    // NOTE: SDL_Quit() calls dlclose(), which can confuse address sanitizer. during asan runs,
    // make sure to define ABEL_ASAN.
    // referencing https://old.reddit.com/r/C_Programming/comments/9kkxax/using_addresssanitizer_with_leaking_libraries/
#ifndef ABEL_ASAN
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
#endif
}

SDL_Renderer *AbelR_getRenderer(void)
{
    return AbelR_state.renderer;
}

struct nk_context *AbelR_getNuklearCtx(void)
{
    return AbelR_state.nkCtx;
}

tAbelR_camera *AbelR_getCamera(void)
{
    return &AbelR_state.camera;
}

tAbelV_iVec2 AbelR_getCameraOffset(void)
{
    tAbelV_iVec2 size = AbelV_newiVec2((AbelR_state.camera.size.x) / 2, (AbelR_state.camera.size.y) / 2);
    tAbelV_iVec2 pos = AbelV_muliVec2(AbelR_state.camera.pos, AbelR_state.scale);

    return AbelV_subiVec2(size, pos);
}

tAbelV_iVec2 AbelR_getScale(void)
{
    return AbelR_state.scale;
}

void AbelR_setScale(tAbelV_iVec2 scale)
{
    AbelR_state.scale = scale;
    // SDL_RenderSetScale(AbelR_state.renderer, scale.x, scale.y);
}

bool AbelR_isVisible(tAbelV_iVec2 pos, tAbelV_iVec2 size)
{
    return (pos.x + size.x >= AbelR_state.camera.pos.x && pos.x <= AbelR_state.camera.pos.x + AbelR_state.camera.size.x) &&
           (pos.y + size.y >= AbelR_state.camera.pos.y && pos.y <= AbelR_state.camera.pos.y + AbelR_state.camera.size.y);
}

void AbelR_zoomCamera(int zoom)
{
    AbelR_setScale(AbelV_addiVec2(AbelR_state.scale, AbelV_newiVec2(zoom, zoom)));
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

tAbelR_texture *AbelR_createText(TTF_Font *font, const char *text)
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Color textColor = {255, 255, 255, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    texture = SDL_CreateTextureFromSurface(AbelR_state.renderer, surface);
    SDL_FreeSurface(surface);
    return AbelR_newTexture(texture);
}

void AbelR_renderTexture(tAbelR_texture *texture, SDL_Rect *src, SDL_Rect *dest)
{
    if (SDL_RenderCopy(AbelR_state.renderer, texture->texture, src, dest))
        ABEL_ERROR("Failed to render tile to target: %s\n", SDL_GetError());
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
