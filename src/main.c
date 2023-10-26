#include "abel.h"
#include "core/init.h"
#include "game.h"

int main()
{
    Abel_init();

    /* run 'game' (ew) */
    AbelG_run();

    Abel_quit();
    return 0;
}