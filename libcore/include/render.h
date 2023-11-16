#ifndef ABEL_RENDER_H
#define ABEL_RENDER_H

#include "abel.h"
#include "core/vec2.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
/* clang-format off */
#include "nuklear.h"
#include "demo/sdl_renderer/nuklear_sdl_renderer.h"
/* clang-format on */

#define START_SCREEN_WIDTH  1024
#define START_SCREEN_HEIGHT 512
#define TILESET_SIZE        16
#define TARGET_FPS          60
#define RENDER_INTERVAL     1000 / TARGET_FPS

typedef struct _tAbelR_texture
{
    SDL_Texture *texture;
    tAbelV_iVec2 size;
} tAbelR_texture;

typedef struct _tAbelR_camera
{
    tAbelV_iVec2 size;
    tAbelV_iVec2 pos;
} tAbelR_camera;

/* initializers */
void AbelR_init(void);
void AbelR_quit(void);

SDL_Renderer *AbelR_getRenderer(void);
struct nk_context *AbelR_getNuklearCtx(void);
tAbelV_iVec2 AbelR_getCameraSize(void);
tAbelV_iVec2 AbelR_getCameraPos(void);
tAbelV_iVec2 AbelR_getCameraOffset(void);
tAbelE_entity *AbelR_getFollow(void);
tAbelV_iVec2 AbelR_getScale(void);
uint32_t AbelR_getFPS(void);

void AbelR_setScale(tAbelV_iVec2 scale);
void AbelR_setCameraSize(tAbelV_iVec2 size);
void AbelR_setCameraPos(tAbelV_iVec2 pos);
void AbelR_setFollow(tAbelE_entity *entity); /* will retain new entity, and release old one*/

bool AbelR_isVisible(tAbelV_iVec2 pos, tAbelV_iVec2 size);
void AbelR_zoomCamera(int zoom);

/* textures */
tAbelR_texture *AbelR_newTexture(SDL_Texture *texture);
tAbelR_texture *AbelR_createText(TTF_Font *font, const char *text);
void AbelR_freeTexture(tAbelR_texture *texture);

void AbelR_renderTexture(tAbelR_texture *texture, SDL_Rect *src, SDL_Rect *dest);
tAbelR_texture *AbelR_newBlankTexture(tAbelV_iVec2 size);

SDL_Rect AbelR_getTileClip(tAbelR_texture *tileSet, TILE_ID id);

/* globals */
extern tAbelV_iVec2 AbelR_tileSize;

#endif