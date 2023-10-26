#ifndef ABEL_RENDER_H
#define ABEL_RENDER_H

#include "abel.h"
#include "core/vec2.h"

#define START_SCREEN_WIDTH  1024
#define START_SCREEN_HEIGHT 512
#define TILESET_SIZE 16


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

SDL_Renderer* AbelR_getRenderer(void);
tAbelR_camera* AbelR_getCamera(void);

/* textures */
tAbelR_texture *AbelR_newTexture(SDL_Texture *texture);
void AbelR_freeTexture(tAbelR_texture *texture);

tAbelR_texture *AbelR_newBlankTexture(tAbelV_iVec2 size);

SDL_Rect AbelR_getTileClip(tAbelR_texture *tileSet, TILE_ID id);
void AbelR_drawClip(tAbelR_texture *texture, SDL_Rect clip, tAbelV_iVec2 pos);

/* globals */
extern tAbelV_iVec2 AbelR_tileSize;

#endif