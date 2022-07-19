#include "assets.h"

#include "core/hashmap.h"
#include "core/serror.h"
#include "layer.h"
#include "render.h"
#include "vec2.h"

ASSET_ID nextAssetID = 0;
struct hashmap *AbelA_assetMap = NULL;

/* ===================================[[ Helper Functions ]]==================================== */

typedef struct
{
    ASSET_ID id;
    ASSET_TYPE type;
    void *data;
} tAbelA_assetElem;

int assetCompare(const void *a, const void *b, void *udata)
{
    const tAbelA_assetElem *ua = a;
    const tAbelA_assetElem *ub = b;

    return ua->id != ub->id;
}

uint64_t assetHash(const void *item, uint64_t seed0, uint64_t seed1)
{
    const tAbelA_assetElem *u = item;
    return (uint64_t)(u->id);
}

void assetFree(void *rawItem)
{
    tAbelA_assetElem *elem = (tAbelA_assetElem *)rawItem;
    
    switch (elem->type) {
    case ASSET_TEXTURE:
        AbelR_freeTexture((tAbelR_texture *)elem->data);
        break;
    case ASSET_FONT:
        TTF_CloseFont((TTF_Font *)elem->data);
        break;
    default:
        ABEL_ERROR("Invalid asset type found while freeing: %d\n", elem->type);
    }
}

/* returned pointer is valid until an entry is added/removed from the hashmap */
tAbelA_assetElem *getAsset(ASSET_ID id)
{
    tAbelA_assetElem *asset =
        (tAbelA_assetElem *)hashmap_get(AbelA_assetMap, &(tAbelA_assetElem){.id = id});
    if (asset == NULL)
        ABEL_ERROR("Failed to get asset with invalid id: %d!\n", id);

    return asset;
}

void *getAssetData(ASSET_ID id)
{
    return getAsset(id)->data;
}

ASSET_ID getNextID(void)
{
    return nextAssetID++;
}

/* =======================================[[ Asset API ]]======================================= */

void AbelA_init(void)
{
    AbelA_assetMap =
        hashmap_new(sizeof(tAbelA_assetElem), 4, 0, 0, assetHash, assetCompare, NULL, NULL);
}

void AbelA_quit(void)
{
    /* free assets */
    hashmap_free(AbelA_assetMap);
}

ASSET_ID AbelA_loadAsset(const char *filePath, ASSET_TYPE type)
{
    void *data;
    ASSET_ID id = getNextID();

    switch (type) {
    case ASSET_TEXTURE: {
        SDL_Texture *rawText;

        /* load raw texture */
        rawText = IMG_LoadTexture(AbelR_renderer, filePath);
        if (rawText == NULL)
            ABEL_ERROR("Failed to load texture from '%s': %s\n", filePath, SDL_GetError());

        data = (void *)AbelR_newTexture(rawText);
        break;
    }
    case ASSET_FONT: {
        TTF_Font *rawFont;

        /* load raw font */
        rawFont = TTF_OpenFont(filePath, 11);
        if (rawFont == NULL)
            ABEL_ERROR("Failed to load font from '%s': %s\n", filePath, SDL_GetError());

        data = (void *)rawFont;
        break;
    }
    default:
        ABEL_ERROR("Invalid asset type provided: %d!\n", type);
    }

    /* insert into assetMap & return id*/
    hashmap_set(AbelA_assetMap, &(tAbelA_assetElem){.id = id, .type = type, .data = data});
    return id;
}

tAbelR_texture *AbelA_getTexture(ASSET_ID id)
{
    return (tAbelR_texture *)getAssetData(id);
}