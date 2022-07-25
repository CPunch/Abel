#include "game.h"

#include "assets.h"
#include "layer.h"
#include "render.h"
#include "sprite.h"

tAbelL_layer *AbelG_layers[ABEL_MAX_LAYERS];
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
        AbelG_layers[i] = AbelL_newLayer(AbelA_getTexture(tilesetID), AbelV_newVec2(16, 16));
    }

    /* TEST AAAAAAAAAAAAAAAAA */

    /* build pretty semi-random grass field */
    for (x = 0; x < 16; x++) {
        for (y = 0; y < 16; y++) {
            switch (rand() % 6) {
            case 0:
                AbelL_drawTile(AbelG_layers[0], AbelL_gridToPos(AbelV_newVec2(x, y)), 0, FRAME_BG);
                break; /* grass */
            case 1:
                AbelL_drawTile(AbelG_layers[0], AbelL_gridToPos(AbelV_newVec2(x, y)), 2, FRAME_BG);
                break; /* weed */
            default:
                AbelL_drawTile(AbelG_layers[0], AbelL_gridToPos(AbelV_newVec2(x, y)), 1, FRAME_BG);
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

void AbelG_run(void)
{
    SDL_Event evnt;
    TTF_Font *font = AbelA_getFont(debugFontID);
    tAbelS_sprite *testSprite;
    int i, animID;
    bool quit = false;

    testSprite = AbelS_newSprite(AbelG_layers[0], AbelL_gridToPos(AbelV_newVec2(2, 2)));
    animID = AbelS_addAnimation(testSprite);
    AbelS_addFrame(testSprite, animID, 16, 1000); /* tile id 16 for 1 second */
    AbelS_addFrame(testSprite, animID, 17, 100);  /* tile id 17 for .1 seconds */
    AbelS_playAnimation(testSprite, animID); /* play animation :D */

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

        /* clear layers */
        SDL_RenderClear(AbelR_renderer);

        /* render layers */
        for (i = 0; i < (sizeof(AbelG_layers) / sizeof(tAbelL_layer *)); i++) {
            AbelL_renderLayer(AbelG_layers[i], NULL);
        }

        /* render to window */
        SDL_RenderPresent(AbelR_renderer);
    }
}