#include "abel.h"
#include "assets.h"
#include "core/init.h"
#include "game.h"
#include "layer.h"
#include "render.h"

int main()
{
    Abel_init();

    /* run 'game' (ew) */
    AbelG_run();

    Abel_quit();
    return 0;
}