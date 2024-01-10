#include <abel.h>
#include <core/init.h>
#include <game.h>
#include <script.h>

int main()
{
    Abel_init();

    int nresults;
    tAbelVM_thread *thread = AbelL_startScript("main.lua", &nresults);
    lua_pop(thread->L, nresults);

    /* run 'game' (ew) */
    AbelG_run();

    AbelL_releaseThread(thread);

    Abel_quit();
    return 0;
}