#include "game.h"

#include "core/tasks.h"
#include "assets.h"
#include "layer.h"
#include "render.h"
#include "entity.h"
#include "sprite.h"

tAbelL_layer *AbelG_layers[LAYER_MAX];
ASSET_ID tilesetID;
ASSET_ID debugFontID;

/* =====================================[[ Initializers ]]====================================== */

void AbelG_init(void)
{
    int i, x, y;

    /* TODO: load this data from game file */
    tilesetID = AbelA_loadAsset("res/tileset.png", ASSET_TEXTURE);
    debugFontID = AbelA_loadAsset("res/kongtext.ttf", ASSET_FONT);
    for (i = 0; i < (sizeof(AbelG_layers) / sizeof(tAbelL_layer *)); i++) {
        AbelG_layers[i] = AbelL_newLayer(AbelA_getTexture(tilesetID), AbelV_newiVec2(16, 16));
    }

    /* TEST AAAAAAAAAAAAAAAAA */

    /* build pretty semi-random grass field */
    for (x = 0; x < 16; x++) {
        for (y = 0; y < 16; y++) {
            switch (rand() % 6) {
            case 0:
                AbelL_drawTile(AbelG_layers[0], AbelL_gridToPos(AbelV_newiVec2(x, y)), 0, FRAME_BG);
                break; /* grass */
            case 1:
                AbelL_drawTile(AbelG_layers[0], AbelL_gridToPos(AbelV_newiVec2(x, y)), 2, FRAME_BG);
                break; /* weed */
            default:
                AbelL_drawTile(AbelG_layers[0], AbelL_gridToPos(AbelV_newiVec2(x, y)), 1, FRAME_BG);
                break; /* empty grass */
            }
        }
    }
}

void AbelG_quit(void)
{
    int i;

    for (i = 0; i < (sizeof(AbelG_layers) / sizeof(tAbelL_layer *)); i++) {
        AbelL_freeLayer(AbelG_layers[i]);
    }
    AbelA_freeAsset(tilesetID);
    AbelA_freeAsset(debugFontID);
}

tAbelL_layer *AbelG_getLayer(LAYER_TYPE layer)
{
    if (layer >= LAYER_MAX)
        return NULL;

    return AbelG_layers[layer];
}

void AbelG_run(void)
{
    SDL_Event evnt;
    TTF_Font *font = AbelA_getFont(debugFontID);
    tAbelE_entity *entity;
    int i, animID;
    bool quit = false;

    entity = AbelE_newEntity(AbelV_i2fVec(AbelL_gridToPos(AbelV_newiVec2(2, 2))));
    animID = AbelS_addAnimation(entity->sprite);
    AbelS_addFrame(entity->sprite, animID, 16, 1000); /* tile id 16 for 1 second */
    AbelS_addFrame(entity->sprite, animID, 17, 100);  /* tile id 17 for .1 seconds */
    AbelS_playAnimation(entity->sprite, animID); /* play animation :D */

    AbelE_setVelocity(entity, AbelV_newfVec2(0, 16));

    /* main engine loop */
    while (!quit) {
        /* handle SDL events */
        while (SDL_PollEvent(&evnt) != 0) {
            switch (evnt.type) {
            case SDL_QUIT:
                quit = true;
                break;
            default:
                break;
            }
        }

        /* run scheduled tasks */
        AbelT_pollTasks();

        /* clear layers */
        SDL_RenderClear(AbelR_renderer);

        /* render layers */
        for (i = 0; i < (sizeof(AbelG_layers) / sizeof(tAbelL_layer *)); i++) {
            AbelL_renderLayer(AbelG_layers[i], NULL);
        }

        /* render to window */
        SDL_RenderPresent(AbelR_renderer);
    }

    AbelE_freeEntity(entity);
}