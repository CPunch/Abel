#include <abel.h>
#include <assets.h>
#include <core/init.h>
#include <game.h>
#include <render.h>
#include <script.h>
#include <world.h>

#define PLAYER_SPEED 32
#define TESTMAP_SIZE 64

static void randMap(void)
{
    int x, y;
    tAbelR_texture *tileSet = AbelA_getTexture("res/tileset.png", AbelV_newiVec2(16, 16));
    AbelW_setTileSet(tileSet);
    AbelR_releaseTexture(tileSet);

    /* build pretty semi-random grass field */
    for (x = 0; x < TESTMAP_SIZE; x++) {
        for (y = 0; y < TESTMAP_SIZE; y++) {
            switch (rand() % 6) {
            case 0:
                AbelW_setCell(AbelV_newiVec2(x, y), 0, false);
                break; /* grass */
            case 1:
                AbelW_setCell(AbelV_newiVec2(x, y), 2, false);
                break; /* weed */
            default:
                AbelW_setCell(AbelV_newiVec2(x, y), 1, false);
                break; /* empty grass */
            }
        }
    }

    AbelW_setCell(AbelV_newiVec2(3, 4), 4, true);
    AbelW_setCell(AbelV_newiVec2(3, 5), 4, true);
    AbelW_setCell(AbelV_newiVec2(3, 6), 4, true);
    AbelW_setCell(AbelV_newiVec2(4, 6), 4, true);
    AbelW_setCell(AbelV_newiVec2(5, 6), 4, true);
}

int main(int argc, char *argv[])
{
    Abel_init(ABEL_INIT_DEFAULT);
    randMap();

    tAbelVM_thread *thread = AbelL_loadScript("main.lua", 0);

    AbelW_updateActiveDistance(1);

    /* run 'game' (ew) */
    AbelG_run();

    /* cleanup */
    if (thread)
        AbelL_releaseThread(thread);
    Abel_quit();
    return 0;
}