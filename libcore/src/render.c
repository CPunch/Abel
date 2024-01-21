#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "render.h"

#include "chunk.h"
#include "core/mem.h"
#include "core/init.h"
#include "core/serror.h"
#include "core/tasks.h"
#include "core/vec2.h"
#include "res/kongtext.h"
#include "entity.h"
#include "world.h"

#define SDL_IMG_FLAGS IMG_INIT_PNG
// #define ABEL_ASAN

typedef struct _tAbelR_State
{
    tAbelR_camera camera;
    tAbelV_iVec2 scale;
    SDL_Window *window;
    SDL_Surface *rendererSurface;
    SDL_Renderer *renderer;
    struct nk_context *nkCtx;
    tAbelT_task *resetFPSTask;
    tAbelT_task *renderTask;
    tAbelE_entity *follow;
    uint32_t FPS;
    uint32_t currFPS;
} tAbelR_state;

static const char WINDOW_TITLE[] = "Abel";
static tAbelR_state AbelR_state = {0};

static void openRenderer(int width, int height, uint32_t flags)
{
    AbelR_setScale(AbelV_newiVec2(2, 2));

    /* init camera */
    AbelR_state.camera.pos = AbelV_newiVec2(0, 0);
    AbelR_state.camera.size = AbelV_newiVec2(width, height);

    if (flags & ABEL_INIT_NOGUI) {
        AbelR_state.rendererSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
        AbelR_state.renderer = SDL_CreateSoftwareRenderer(AbelR_state.rendererSurface);
    } else {
        /* open window */
        AbelR_state.window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (AbelR_state.window == NULL)
            ABEL_ERROR("Failed to open window: %s\n", SDL_GetError());

        /* create & set rendering target */
        AbelR_state.renderer = SDL_CreateRenderer(AbelR_state.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        if (AbelR_state.renderer == NULL)
            ABEL_ERROR("Failed to create renderer target: %s\n", SDL_GetError());
    }

    SDL_SetRenderTarget(AbelR_state.renderer, NULL);
}

static void drawRealtimeStats(void)
{
    struct nk_context *ctx = AbelR_getNuklearCtx();

    /* windowless watermark for realtime stats */
    nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_hide());
    if (nk_begin(ctx, "DEBUG", nk_rect(0, 0, 210, 120), NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_static(ctx, 13, 150, 1);
        nk_labelf(ctx, NK_TEXT_LEFT, "ABEL v0.1");
        nk_layout_row_static(ctx, 13, 200, 1);
        nk_labelf(ctx, NK_TEXT_LEFT, "FPS: %d", AbelR_getFPS());
    }
    nk_end(ctx);
    nk_style_pop_style_item(ctx);
}

static uint32_t resetFPSTask(uint32_t delta, void *uData)
{
    AbelR_state.FPS = AbelR_state.currFPS;
    AbelR_state.currFPS = 0;
    return 1000;
}

static uint32_t renderTask(uint32_t delta, void *uData)
{
    drawRealtimeStats();

    if (AbelR_state.follow) {
        AbelR_setCameraPos(AbelV_addiVec2(AbelV_f2iVec(AbelR_state.follow->sprite.pos), AbelV_newiVec2(8, 8)));
    }

    /* clear layers */
    SDL_RenderClear(AbelR_state.renderer);

    /* render chunks */
    AbelW_render();
    nk_sdl_render(NK_ANTI_ALIASING_ON);

    /* present to window */
    SDL_RenderPresent(AbelR_state.renderer);
    AbelR_state.currFPS++;

    return RENDER_INTERVAL;
}

/* =====================================[[ Initializers ]]====================================== */

static void reset()
{
    AbelR_state.window = NULL;
    AbelR_state.rendererSurface = NULL;
    AbelR_state.renderer = NULL;
    AbelR_state.nkCtx = NULL;
    AbelR_state.resetFPSTask = NULL;
    AbelR_state.renderTask = NULL;
    AbelR_state.follow = NULL;
}

void AbelR_init(uint32_t initFlags)
{
    reset();

    /* pretty sure sdl_img & ttf_img are initalized with this,,, but i think the API is for
        previous versions of SDL so... we'll go ahead and do it anyways */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
        ABEL_ERROR("Failed to initialize SDL: %s\n", SDL_GetError());

    /* IMG_Init() returns the flags set, make sure they match */
    if (IMG_Init(SDL_IMG_FLAGS) != SDL_IMG_FLAGS)
        ABEL_ERROR("Failed to initialize SDL_IMG: %s\n", IMG_GetError());

    if (TTF_Init() != 0)
        ABEL_ERROR("Failed to initialize: SDL_TTF: %s\n", TTF_GetError());

    openRenderer(START_SCREEN_WIDTH, START_SCREEN_HEIGHT, initFlags);

    /* setup nuklear ui */
    AbelR_state.nkCtx = nk_sdl_init(AbelR_state.window, AbelR_state.renderer);

    {
        struct nk_font_atlas *atlas;
        struct nk_font_config config = nk_font_config(0);
        struct nk_font *font;

        nk_sdl_font_stash_begin(&atlas);

        /* load default font */
        font = nk_font_atlas_add_from_memory(atlas, ABEL_KONGTEXTBLOB, sizeof(ABEL_KONGTEXTBLOB), 14, &config);
        /* font = nk_font_atlas_add_from_file(atlas, "res/kongtext.ttf", 14, &config); */
        nk_sdl_font_stash_end();

        nk_style_set_font(AbelR_state.nkCtx, &font->handle);
    }

    AbelR_state.resetFPSTask = AbelT_newTask(1000, resetFPSTask, NULL);
    AbelR_state.renderTask = AbelT_newTask(RENDER_INTERVAL, renderTask, NULL);
}

void AbelR_quit(void)
{
    AbelT_freeTask(AbelR_state.resetFPSTask);
    AbelT_freeTask(AbelR_state.renderTask);

    nk_sdl_shutdown();
    SDL_DestroyRenderer(AbelR_state.renderer);
    SDL_FreeSurface(AbelR_state.rendererSurface);
    SDL_DestroyWindow(AbelR_state.window);

    if (AbelR_state.follow)
        AbelM_releaseRef(&AbelR_state.follow->refCount);

    /* asan won't actually mark any globally stored pointers as
     memory leaks */
    reset();

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

tAbelV_iVec2 AbelR_getCameraPos(void)
{
    return AbelR_state.camera.pos;
}

tAbelV_iVec2 AbelR_getCameraSize(void)
{
    return AbelR_state.camera.size;
}

tAbelV_iVec2 AbelR_getCameraOffset(void)
{
    tAbelV_iVec2 size = AbelV_newiVec2((AbelR_state.camera.size.x) / 2, (AbelR_state.camera.size.y) / 2);
    tAbelV_iVec2 pos = AbelV_muliVec2(AbelR_state.camera.pos, AbelR_state.scale);

    return AbelV_subiVec2(size, pos);
}

tAbelE_entity *AbelR_getFollow(void)
{
    return AbelR_state.follow;
}

tAbelV_iVec2 AbelR_getScale(void)
{
    return AbelR_state.scale;
}

uint32_t AbelR_getFPS(void)
{
    return AbelR_state.FPS;
}

void AbelR_setScale(tAbelV_iVec2 scale)
{
    AbelR_state.scale = scale;
}

void AbelR_setCameraSize(tAbelV_iVec2 size)
{
    AbelR_state.camera.size = size;
}

void AbelR_setCameraPos(tAbelV_iVec2 pos)
{
    tAbelV_iVec2 gridPos = AbelW_posToGrid(pos);
    tAbelV_iVec2 chunkPos = AbelW_getChunkPos(gridPos);

    AbelW_updateActiveChunkPos(chunkPos);
    AbelR_state.camera.pos = pos;
}

void AbelR_setFollow(tAbelE_entity *entity)
{
    if (AbelR_state.follow)
        AbelM_releaseRef(&AbelR_state.follow->refCount);

    if (entity)
        AbelM_retainRef(&entity->refCount);

    AbelR_state.follow = entity;
}

bool AbelR_isVisible(tAbelV_iVec2 pos, tAbelV_iVec2 size)
{
    return (pos.x + size.x >= AbelR_state.camera.pos.x && pos.x <= AbelR_state.camera.pos.x + AbelR_state.camera.size.x) &&
           (pos.y + size.y >= AbelR_state.camera.pos.y && pos.y <= AbelR_state.camera.pos.y + AbelR_state.camera.size.y);
}

void AbelR_zoomCamera(int zoom)
{
    tAbelV_iVec2 newScale = AbelV_addiVec2(AbelR_state.scale, AbelV_newiVec2(zoom, zoom));

    /* validate new scale */
    if (newScale.x < 1 || newScale.y < 1)
        return;

    AbelR_setScale(newScale);
}

/* ======================================[[ Texture API ]]====================================== */

static void freeTexture(tAbelM_refCount *ref)
{
    tAbelR_texture *texture = (tAbelR_texture *)ref;
    SDL_DestroyTexture(texture->texture);
    AbelM_free(texture);
}

tAbelR_texture *AbelR_newTexture(SDL_Texture *rawTexture, tAbelV_iVec2 tileSize)
{
    tAbelR_texture *texture = (tAbelR_texture *)AbelM_malloc(sizeof(tAbelR_texture));
    texture->texture = rawTexture;

    /* grab texture size */
    if (SDL_QueryTexture(rawTexture, NULL, NULL, (int *)&texture->size.x, (int *)&texture->size.y) != 0)
        ABEL_ERROR("Failed to query texture information: %s\n", SDL_GetError());

    /* set tilset size */
    if (tileSize.x == 0 && tileSize.y == 0) {
        tileSize = texture->size;
    }
    texture->tileSize = tileSize;

    /* make sure we can render textures *on top of* others, keep transparency */
    SDL_SetTextureBlendMode(rawTexture, SDL_BLENDMODE_BLEND);
    AbelM_initRef(&texture->refCount, freeTexture);
    return texture;
}

void AbelR_releaseTexture(tAbelR_texture *texture)
{
    AbelM_releaseRef(&texture->refCount);
}

void AbelR_retainTexture(tAbelR_texture *texture)
{
    AbelM_retainRef(&texture->refCount);
}

tAbelR_texture *AbelR_createText(TTF_Font *font, const char *text)
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Color textColor = {255, 255, 255, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    texture = SDL_CreateTextureFromSurface(AbelR_state.renderer, surface);
    SDL_FreeSurface(surface);
    return AbelR_newTexture(texture, AbelV_newiVec2(0, 0));
}

void AbelR_renderTexture(tAbelR_texture *texture, SDL_Rect *src, SDL_Rect *dest)
{
    if (SDL_RenderCopy(AbelR_state.renderer, texture->texture, src, dest))
        ABEL_ERROR("Failed to render tile to target: %s\n", SDL_GetError());
}

tAbelR_texture *AbelR_newBlankTexture(tAbelV_iVec2 size, tAbelV_iVec2 tileSize)
{
    SDL_Texture *rawTexture;

    /* create SDL texture */
    rawTexture = SDL_CreateTexture(AbelR_state.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.x, size.y);
    if (rawTexture == NULL)
        ABEL_ERROR("Failed to make blank texture: %s\n", SDL_GetError());

    /* create Abel texture */
    return AbelR_newTexture(rawTexture, tileSize);
}

SDL_Rect AbelR_getTileClip(tAbelR_texture *tileSet, TILE_ID id)
{
    tAbelV_iVec2 cordSize;
    int x, y;

    /* grabs the x/y cords of our tile in our texture */
    cordSize = AbelV_diviVec2(tileSet->size, tileSet->tileSize);
    y = id / cordSize.x;
    x = id % cordSize.x;

    if (y > cordSize.y)
        ABEL_ERROR("Invalid tile id: %d\n", id);

    /* return clip of texture */
    return (SDL_Rect){.x = x * tileSet->tileSize.x, .y = y * tileSet->tileSize.y, .w = tileSet->tileSize.x, .h = tileSet->tileSize.y};
}
