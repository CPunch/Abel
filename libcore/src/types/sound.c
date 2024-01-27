#include "sound.h"

#include "types/sound.h"

static const char *ABEL_SOUND_METATABLE = "Sound";

static int soundGC(lua_State *L)
{
    tAbelR_sound *t = AbelL_toSound(L, 1);
    AbelR_releaseSound(t);
    return 0;
}

static luaL_Reg soundMetaMethods[] = {
    {"__gc", soundGC},
    {  NULL,    NULL}
};

static int soundPlay(lua_State *L)
{
    tAbelR_sound *t = AbelL_toSound(L, 1);
    int loops = luaL_optinteger(L, 2, -1);
    AbelR_playSound(t, loops);
    return 0;
}

static luaL_Reg soundMethods[] = {
    {"Play", soundPlay},
    {  NULL,      NULL}
};

static int soundLoad(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);
    tAbelR_sound *t = AbelR_loadSound(path);
    AbelL_pushSound(L, t);
    AbelR_releaseSound(t);
    return 1;
}

void AbelL_registerSound(lua_State *L)
{
    luaL_newmetatable(L, ABEL_SOUND_METATABLE);
    luaL_setfuncs(L, soundMetaMethods, 0);

    luaL_newlib(L, soundMethods);
    lua_setfield(L, -2, "__index");

    lua_pop(L, 1);

    lua_newtable(L);
    lua_pushcfunction(L, soundLoad);
    lua_setfield(L, -2, "Load");
    lua_setglobal(L, "Sound");
}

void AbelL_pushSound(lua_State *L, tAbelR_sound *t)
{
    tAbelR_sound **u = lua_newuserdata(L, sizeof(tAbelR_sound *));
    *u = t;
    luaL_setmetatable(L, ABEL_SOUND_METATABLE);
    AbelM_retainRef(&t->refCount);
}

tAbelR_sound *AbelL_toSound(lua_State *L, int index)
{
    tAbelR_sound **u = luaL_checkudata(L, index, ABEL_SOUND_METATABLE);
    return *u;
}