#include "abel.h"
#include "assets.h"
#include "core/init.h"
#include "layer.h"
#include "render.h"

int main()
{
    tAbelL_layer *layer;
    ASSET_ID tilesetID;
    int x, y;

    Abel_init();

    tilesetID = AbelA_loadAsset("res/tileset.png", ASSET_TEXTURE);
    layer = AbelL_newLayer(AbelA_getTexture(tilesetID), AbelV_newVec2(16, 16));

    /* build pretty semi-random grass field */
    for (x = 0; x < 16; x++) {
        for (y = 0; y < 16; y++) {
            switch (rand() % 6) {
            case 0: AbelL_addBGTile(layer, AbelV_newVec2(x, y), 0); break; /* grass */
            case 1: AbelL_addBGTile(layer, AbelV_newVec2(x, y), 2); break; /* weed */
            default:AbelL_addBGTile(layer, AbelV_newVec2(x, y), 1); break; /* empty grass */
            }
        }
    }

    /* render to window, pause for 6 seconds and quit */
    AbelL_renderLayer(layer, NULL);
    SDL_RenderPresent(AbelR_renderer);
    SDL_Delay(6000);
    Abel_quit();
    return 0;
}