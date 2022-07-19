#ifndef ABEL_ASSETS_H
#define ABEL_ASSETS_H

#include "abel.h"

typedef uint16_t ASSET_ID;

typedef enum
{
    ASSET_TILESET
} ASSET_TYPE;

void AbelA_init(void);
void AbelA_quit(void);

/* NOTE: AbelA_init() must be called before any of the functions below */

/* on error, returns ABEL_INVALID_ASSET */
ASSET_ID AbelA_loadAsset(const char *filePath, ASSET_TYPE type);

tAbelT_tileSet *AbelA_getTileSet(ASSET_ID id);

#endif