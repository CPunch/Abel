#include <abel.h>
#include <assets.h>
#include <core/init.h>
#include <game.h>
#include <render.h>
#include <script.h>
#include <world.h>

int main(int argc, char *argv[])
{
    Abel_init(ABEL_INIT_DEFAULT);
    AbelW_updateActiveDistance(1);

    {
        tAbelVM_thread *thread = AbelL_loadScript("main.lua", 0);

        /* run 'game' (ew) */
        AbelG_run();

        /* AbelL_loadScript can return a NULL if the script failed :( */
        if (thread)
            AbelL_releaseThread(thread);   
    }

    Abel_quit();
    return 0;
}