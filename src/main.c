#include "abel.h"
#include "assets.h"
#include "core/init.h"
#include "layer.h"
#include "render.h"

int main()
{
    tAbelL_layer *layer;
    ASSET_ID tilesetID;

    Abel_init();

    tilesetID = AbelA_loadAsset("res/tileset.png", ASSET_TEXTURE);
    layer = AbelL_newLayer(AbelA_getTexture(tilesetID), AbelV_newVec2(16, 16));

    AbelL_addBGTile(layer, AbelV_newVec2(0, 0), 0);
    AbelL_addBGTile(layer, AbelV_newVec2(1, 0), 1);
    AbelL_addBGTile(layer, AbelV_newVec2(2, 0), 2);

    AbelL_addBGTile(layer, AbelV_newVec2(0, 1), 0);
    AbelL_addBGTile(layer, AbelV_newVec2(1, 1), 1);
    AbelL_addBGTile(layer, AbelV_newVec2(2, 1), 2);

    AbelL_renderLayer(layer, NULL);
    SDL_RenderPresent(AbelR_renderer);
    SDL_Delay(6000);
    Abel_quit();
    return 0;
}