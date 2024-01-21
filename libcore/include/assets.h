#ifndef ABEL_ASSETS_H
#define ABEL_ASSETS_H

#include "abel.h"
#include "core/vec2.h"

/* initalizers */
void AbelA_init(uint32_t initFlags);
void AbelA_quit(void);

tAbelR_texture *AbelA_getTexture(const char *path, tAbelV_iVec2 tileSize);

#endif