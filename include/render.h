#ifndef ABEL_RENDER_H
#define ABEL_RENDER_H

#include "abel.h"
#include "vec2.h"

typedef struct _tAbelR_texture {
    SDL_Texture *texture;
    tAbel_vec2 size;
} tAbelR_texture;

void AbelR_init(void);
void AbelR_quit(void);

tAbelR_texture *AbelR_newTexture(SDL_Texture *texture);
void AbelR_freeTexture(tAbelR_texture *texture);

extern SDL_Window *AbelR_window;
extern SDL_Renderer *AbelR_renderer;

#endif