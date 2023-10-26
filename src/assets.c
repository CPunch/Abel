#include "assets.h"

#include "core/hashmap.h"
#include "core/mem.h"
#include "core/serror.h"
#include "core/vec2.h"
#include "render.h"

/* ===================================[[ Helper Functions ]]==================================== */

typedef struct
{
    void *data;
    const char *path;
    ASSET_TYPE type;
    ASSET_ID id;
} tAbelA_asset;

AbelM_newVector(tAbelA_asset, AbelA_assetTable);

/* valid until asset is added/removed */
static tAbelA_asset *getAssetByID(ASSET_ID id)
{
    if (id >= AbelM_countVector(AbelA_assetTable))
        return NULL;

    /* make sure we don't return an invalid asset */
    return AbelA_assetTable[id].type != ASSET_NONE ? &AbelA_assetTable[id] : NULL;
}

static tAbelA_asset *getAssetByPath(const char *path)
{
    int id;

    /* search loaded assets (this is the *only* compromise for this approach imo,
        this is only called when AbelA_loadAsset() is called basically. since this
        is only ran on startup, using something like a hashmap is overkill.) */
    for (id = 0; id < AbelM_countVector(AbelA_assetTable); id++)
        if (strcmp(AbelA_assetTable[id].path, path) == 0)
            return &AbelA_assetTable[id];

    return NULL;
}

static void *getAssetData(ASSET_ID id)
{
    tAbelA_asset *asset = getAssetByID(id);
    if (asset == NULL)
        ABEL_ERROR("Failed to get loaded asset with invalid ID: %d\n", id);

    return asset->data;
}

static ASSET_ID getNextAssetID(void)
{
    int id;

    /* search for empty spot in asset table */
    for (id = 0; id < AbelM_countVector(AbelA_assetTable); id++)
        if (AbelA_assetTable[id].type == ASSET_NONE)
            break;

    return id;
}

static ASSET_ID AbelA_insertAsset(void *data, const char *path, ASSET_TYPE type)
{
    ASSET_ID nextID = getNextAssetID();

    /* check if we need to grow the asset table */
    if (nextID == AbelM_countVector(AbelA_assetTable)) {
        AbelM_growVector(tAbelA_asset, AbelA_assetTable, 1);
        ++AbelM_countVector(AbelA_assetTable);
    }

    /* insert into asset table */
    AbelA_assetTable[nextID] = (tAbelA_asset){
        .path = path,
        .data = data,
        .type = type,
        .id = nextID,
    };

    /* return id */
    return nextID;
}

/* =====================================[[ Initializers ]]====================================== */

void AbelA_init(void)
{
    AbelM_initVector(AbelA_assetTable, 4);
}

void AbelA_quit(void)
{
    int i;

    for (i = 0; i < AbelM_countVector(AbelA_assetTable); i++) {
        if (AbelA_assetTable[i].type != ASSET_NONE)
            AbelA_freeAsset(i);
    }
}

/* =======================================[[ Asset API ]]======================================= */

ASSET_ID AbelA_loadAsset(const char *path, ASSET_TYPE type)
{
    tAbelA_asset *asset;
    void *data;

    /* first, check if we already loaded this asset */
    if ((asset = getAssetByPath(path)) != NULL)
        return asset->id;

    switch (type) {
    case ASSET_TEXTURE: {
        SDL_Texture *rawText;

        /* load raw texture */
        rawText = IMG_LoadTexture(AbelR_getRenderer(), path);
        if (rawText == NULL)
            ABEL_ERROR("Failed to load texture from '%s': %s\n", path, SDL_GetError());

        data = (void *)AbelR_newTexture(rawText);
        break;
    }
    case ASSET_FONT: {
        TTF_Font *rawFont;

        /* load raw font */
        rawFont = TTF_OpenFont(path, 11);
        if (rawFont == NULL)
            ABEL_ERROR("Failed to load font from '%s': %s\n", path, SDL_GetError());

        data = (void *)rawFont;
        break;
    }
    default:
        ABEL_ERROR("Invalid asset type provided: %d!\n", type);
    }

    /* insert into asset table & return id*/
    return AbelA_insertAsset(data, path, type);
}

void AbelA_freeAsset(ASSET_ID id)
{
    tAbelA_asset *asset = getAssetByID(id);

    if (asset == NULL)
        ABEL_ERROR("Failed to free invalid asset ID: %d\n", id);

    switch (asset->type) {
    case ASSET_TEXTURE:
        AbelR_freeTexture((tAbelR_texture *)asset->data);
        break;
    case ASSET_FONT:
        TTF_CloseFont((TTF_Font *)asset->data);
        break;
    default:
        ABEL_ERROR("Invalid asset type found while freeing: %d\n", asset->type);
    }

    /* mark id as unused */
    asset->type = ASSET_NONE;
}

/* =======================================[[ Getters ]]========================================= */

tAbelR_texture *AbelA_getTexture(ASSET_ID id)
{
    return (tAbelR_texture *)getAssetData(id);
}

TTF_Font *AbelA_getFont(ASSET_ID id)
{
    return (TTF_Font *)getAssetData(id);
}