#ifndef ABEL_RENDER_H
#define ABEL_RENDER_H

#include "abel.h"
#include "vec2.h"

#define START_SCREEN_WIDTH  1024
#define START_SCREEN_HEIGHT 512

typedef struct _tAbelR_texture
{
    SDL_Texture *texture;
    tAbel_vec2 size;
} tAbelR_texture;

/* initializers */
void AbelR_init(void);
void AbelR_quit(void);

/* textures */
tAbelR_texture *AbelR_newTexture(SDL_Texture *texture);
void AbelR_freeTexture(tAbelR_texture *texture);

tAbelR_texture *AbelR_newBlankTexture(tAbel_vec2 size);

/* globals */
extern SDL_Window *AbelR_window;
extern SDL_Renderer *AbelR_renderer;
extern tAbel_vec2 AbelR_windowSize;
extern tAbel_vec2 AbelR_camera;

#endif