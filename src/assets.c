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

typedef struct _tAbelA_state
{
    AbelM_newVector(tAbelA_asset, assetTable);
} tAbelA_state;

static tAbelA_state AbelA_state;

/* valid until asset is added/removed */
static tAbelA_asset *getAssetByID(ASSET_ID id)
{
    if (id >= AbelM_countVector(AbelA_state.assetTable))
        return NULL;

    /* make sure we don't return an invalid asset */
    return AbelA_state.assetTable[id].type != ASSET_NONE ? &AbelA_state.assetTable[id] : NULL;
}

static tAbelA_asset *getAssetByPath(const char *path)
{
    int id;

    /* search loaded assets (this is the *only* compromise for this approach imo,
        this is only called when AbelA_loadAsset() is called basically. since this
        is only ran on startup, using something like a hashmap is overkill.) */
    for (id = 0; id < AbelM_countVector(AbelA_state.assetTable); id++)
        if (strcmp(AbelA_state.assetTable[id].path, path) == 0)
            return &AbelA_state.assetTable[id];

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
    for (id = 0; id < AbelM_countVector(AbelA_state.assetTable); id++)
        if (AbelA_state.assetTable[id].type == ASSET_NONE)
            break;

    return id;
}

static void *loadData(const char *path, ASSET_TYPE type)
{
    void *data;

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

    return data;
}

static ASSET_ID insertAsset(void *data, const char *path, ASSET_TYPE type)
{
    ASSET_ID nextID = getNextAssetID();

    /* check if we need to grow the asset table */
    if (nextID == AbelM_countVector(AbelA_state.assetTable)) {
        AbelM_growVector(tAbelA_asset, AbelA_state.assetTable, 1);
        ++AbelM_countVector(AbelA_state.assetTable);
    }

    /* insert into asset table */
    AbelA_state.assetTable[nextID] = (tAbelA_asset){
        .path = path,
        .data = data,
        .type = type,
        .id = nextID,
    };

    /* return id */
    return nextID;
}

static void insertReservedAsset(void *data, const char *path, ASSET_ID id, ASSET_TYPE type)
{
    /* check if we need to grow the asset table */
    int count = AbelM_countVector(AbelA_state.assetTable);
    if (id >= count) {
        AbelM_growVector(tAbelA_asset, AbelA_state.assetTable, (id - count) + 1);
        AbelM_countVector(AbelA_state.assetTable) += (id - count) + 1;

        /* set ASSET_NONE on all new entries */
        for (int i = count; i < AbelM_countVector(AbelA_state.assetTable); i++)
            AbelA_state.assetTable[i].type = ASSET_NONE;

    } else {
        /* make sure we don't overwrite an existing asset */
        if (AbelA_state.assetTable[id].type != ASSET_NONE)
            ABEL_ERROR("Failed to load reserved asset: %d, asset already exists!\n", id);
    }

    /* insert into asset table */
    AbelA_state.assetTable[id] = (tAbelA_asset){
        .path = path,
        .data = data,
        .type = type,
        .id = id,
    };
}

static void loadReservedAsset(ASSET_ID id, const char *path, ASSET_TYPE type)
{
    void *data = loadData(path, type);
    insertReservedAsset(data, path, id, type);
}

/* =====================================[[ Initializers ]]====================================== */

void AbelA_init(void)
{
    AbelM_initVector(AbelA_state.assetTable, 4);

    /* load and insert reserved assets;
       these assets are expected to be populated and *always*
       available to the rest of the codebase */
    loadReservedAsset(ASSET_ID_MAP_TILESET, "res/tileset.png", ASSET_TEXTURE);
    loadReservedAsset(ASSET_ID_ENTITY_TILESET, "res/tileset.png", ASSET_TEXTURE);
}

void AbelA_quit(void)
{
    int i;

    for (i = 0; i < AbelM_countVector(AbelA_state.assetTable); i++) {
        if (AbelA_state.assetTable[i].type != ASSET_NONE)
            AbelA_freeAsset(i);
    }

    AbelM_free(AbelA_state.assetTable);
}

/* =======================================[[ Asset API ]]======================================= */

ASSET_ID AbelA_loadAsset(const char *path, ASSET_TYPE type)
{
    tAbelA_asset *asset;
    void *data;

    /* first, check if we already loaded this asset */
    if ((asset = getAssetByPath(path)) != NULL)
        return asset->id;

    data = loadData(path, type);

    /* insert into asset table & return id*/
    return insertAsset(data, path, type);
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