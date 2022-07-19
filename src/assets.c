#include "assets.h"

#include "core/hashmap.h"
#include "render.h"
#include "core/serror.h"
#include "tileset.h"
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

/* returned pointer is valid until an entry is added/removed from the hashmap */
tAbelA_assetElem *getAsset(ASSET_ID id)
{
    tAbelA_assetElem *asset =
        (tAbelA_assetElem *)hashmap_get(AbelA_assetMap, &(tAbelA_assetElem){.id = id});
    if (asset == NULL)
        ABEL_ERROR("Failed to get asset with invalid id: %d!\n", id);

    return asset;
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
}

ASSET_ID AbelA_loadAsset(const char *filePath, ASSET_TYPE type)
{
    void *data;
    ASSET_ID id = getNextID();

    switch (type) {
    case ASSET_TILESET: {
        SDL_Texture *rawText;

        /* load raw texture */
        rawText = IMG_LoadTexture(AbelR_renderer, filePath);
        if (rawText == NULL)
            ABEL_ERROR("Failed to load texture from '%s': %s\n", filePath, SDL_GetError());

        /* create tileset */
        data = AbelT_newTileSet(AbelR_newTexture(rawText), AbelV_newVec2(16, 16));
    }
    default:
        ABEL_ERROR("Invalid asset type provided: %d!\n", type);
    }

    /* insert into assetMap & return id*/
    hashmap_set(AbelA_assetMap, &(tAbelA_assetElem){.id = id, .type = type, .data = data});
    return id;
}

SDL_Texture *AbelA_getTexture(ASSET_ID id)
{
}