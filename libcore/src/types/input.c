#include "types/input.h"

#include "input.h"
#include "script.h"

static void lua_onKeyDown(const void *uData, const void *eventData)
{
    tAbelVM_luaEvent *userData = (tAbelVM_luaEvent *)uData;
    SDL_Event *evnt = (SDL_Event *)eventData;

    lua_pushfstring(userData->thread->L, "%s", SDL_GetKeyName(evnt->key.keysym.sym));
    AbelL_callFunction(userData->thread, 1, 0);
}

static void lua_onKeyUp(const void *uData, const void *eventData)
{
    tAbelVM_luaEvent *userData = (tAbelVM_luaEvent *)uData;
    SDL_Event *evnt = (SDL_Event *)eventData;

    lua_pushfstring(userData->thread->L, "%s", SDL_GetKeyName(evnt->key.keysym.sym));
    AbelL_callFunction(userData->thread, 1, 0);
}

static int onKeyDown(lua_State *L)
{
    AbelL_connectEvent(AbelL_getThread(L), AbelI_onKeyDownHead(), lua_onKeyDown, NULL);
    return 1;
}

static int onKeyUp(lua_State *L)
{
    AbelL_connectEvent(AbelL_getThread(L), AbelI_onKeyUpHead(), lua_onKeyUp, NULL);
    return 1;
}

static luaL_Reg inputMethods[] = {
    {  "OnKeyUp",   onKeyUp},
    {"OnKeyDown", onKeyDown},
    {       NULL,      NULL}
};

typedef struct
{
    const char *name;
    int value;
} tAbelL_Enum;

static tAbelL_Enum SDLKeyEnum[] = {
    {   "Up",    SDLK_UP},
    { "Down",  SDLK_DOWN},
    { "Left",  SDLK_LEFT},
    {"Right", SDLK_RIGHT},
    {   NULL,          0}
};

void AbelL_registerInput(lua_State *L)
{
    lua_newtable(L);
    luaL_setfuncs(L, inputMethods, 0);
    lua_setglobal(L, "Input");

    lua_newtable(L);
    for (int i = 0; SDLKeyEnum[i].name != NULL; i++) {
        lua_pushinteger(L, SDLKeyEnum[i].value);
        lua_setfield(L, -2, SDLKeyEnum[i].name);
    }
    lua_setglobal(L, "Key");
}