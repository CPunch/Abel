#ifndef ABEL_ASSETS_H
#define ABEL_ASSETS_H

#include "abel.h"
#include "core/vec2.h"

/* initalizers */
void AbelA_init(uint32_t initFlags);
void AbelA_quit(void);

bool AbelA_openArchive(const char *path);

tAbelR_texture *AbelA_getTexture(const char *path, tAbelV_iVec2 tileSize);
tAbelK_sound *AbelA_getSound(const char *path);

#endif