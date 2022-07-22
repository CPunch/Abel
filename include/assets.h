#ifndef ABEL_ASSETS_H
#define ABEL_ASSETS_H

#include "abel.h"

typedef uint16_t ASSET_ID;

typedef enum
{
    ASSET_NONE,
    ASSET_TEXTURE,
    ASSET_FONT,
} ASSET_TYPE;

/* initalizers */
void AbelA_init(void);
void AbelA_quit(void);

/* assets */
ASSET_ID AbelA_loadAsset(const char *filePath, ASSET_TYPE type);
void AbelA_freeAsset(ASSET_ID id);

/* getters */
tAbelR_texture *AbelA_getTexture(ASSET_ID id);
TTF_Font *AbelA_getFont(ASSET_ID id);

#endif