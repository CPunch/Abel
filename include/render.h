#ifndef ABEL_RENDER_H
#define ABEL_RENDER_H

#include "abel.h"
#include "core/vec2.h"

#define START_SCREEN_WIDTH  1024
#define START_SCREEN_HEIGHT 512
#define TILESET_SIZE        16

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
tAbelR_camera *AbelR_getCamera(void);
tAbelV_iVec2 AbelR_getCameraOffset(void);
tAbelV_iVec2 AbelR_getScale(void);

void AbelR_setScale(tAbelV_iVec2 scale);

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