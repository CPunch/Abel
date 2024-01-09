#include <abel.h>
#include <core/init.h>
#include <game.h>
#include <script.h>

int main()
{
    Abel_init();

    tAbelVM_thread *thread = AbelL_startScript("main.lua");

    /* run 'game' (ew) */
    AbelG_run();

    AbelL_releaseThread(thread);

    Abel_quit();
    return 0;
}