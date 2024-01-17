#include "types/world.h"

#include "script.h"
#include "world.h"

static void lua_onStep(const void *uData, const void *eventData)
{
    tAbelVM_luaEvent *userData = (tAbelVM_luaEvent *)uData;

    AbelL_callFunction(userData->thread, 0, 0);
}

static int onStep(lua_State *L)
{
    AbelL_connectEvent(AbelL_getThread(L), AbelW_onStepHead(), lua_onStep, NULL);
    return 1;
}

static luaL_Reg worldMethods[] = {
    {"onStep", onStep},
    {    NULL,   NULL}
};

void AbelL_registerWorld(lua_State *L)
{
    lua_newtable(L);
    luaL_setfuncs(L, worldMethods, 0);
    lua_setglobal(L, "World");
}