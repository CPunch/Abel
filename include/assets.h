#ifndef ABEL_ASSETS_H
#define ABEL_ASSETS_H

#include "abel.h"

typedef enum
{
    ASSET_NONE,
    ASSET_TEXTURE,
    ASSET_FONT,
} ASSET_TYPE;

enum
{
    ASSET_ID_MAP_TILESET,
    ASSET_ID_SPRITE_TILESET,
    _ASSET_ID_DYNAMIC_START,
};

/* initalizers */
void AbelA_init(void);
void AbelA_quit(void);

/* assets */
ASSET_ID AbelA_loadAsset(const char *filePath, ASSET_TYPE type);
void AbelA_loadReservedAsset(ASSET_ID id, const char *filePath, ASSET_TYPE type);
void AbelA_freeAsset(ASSET_ID id);

/* getters */
tAbelR_texture *AbelA_getTexture(ASSET_ID id);
TTF_Font *AbelA_getFont(ASSET_ID id);

#endif