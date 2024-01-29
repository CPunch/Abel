#include "types/world.h"

#include "script.h"
#include "types/entity.h"
#include "types/texture.h"
#include "types/vec2.h"
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

static int worldPosToGrid(lua_State *L)
{
    tAbelV_iVec2 pos = AbelV_f2iVec(AbelL_checkVec2(L, 1));
    AbelL_pushVec2(L, AbelV_i2fVec(AbelW_posToGrid(pos)));
    return 1;
}

static int worldGridToPos(lua_State *L)
{
    tAbelV_iVec2 gridPos = AbelV_f2iVec(AbelL_checkVec2(L, 1));
    AbelL_pushVec2(L, AbelV_i2fVec(AbelW_gridToPos(gridPos)));
    return 1;
}

static int worldSetTileSet(lua_State *L)
{
    tAbelR_texture *tileSet = AbelL_toTexture(L, 1);
    AbelW_setTileSet(tileSet);
    return 0;
}

static int worldGetTileSet(lua_State *L)
{
    AbelL_pushTexture(L, AbelW_getTileSet());
    return 1;
}

static TILE_ID checkTileID(lua_State *L, int index)
{
    if (lua_type(L, index) == LUA_TNIL) {
        return TILE_NIL;
    } else {
        return luaL_checkinteger(L, index);
    }
}

static int worldSetCell(lua_State *L)
{
    tAbelV_iVec2 pos = AbelV_f2iVec(AbelL_checkVec2(L, 1));
    TILE_ID bg = checkTileID(L, 2);
    TILE_ID fg = checkTileID(L, 3);
    bool isSolid = lua_toboolean(L, 4);

    AbelW_setCell(pos, bg, fg, isSolid);
    return 0;
}

static int worldGetCell(lua_State *L)
{
    tAbelV_iVec2 pos = AbelV_f2iVec(AbelL_checkVec2(L, 1));
    tAbelW_cell cell = AbelW_getCell(pos);

    lua_pushinteger(L, cell.bgID);
    lua_pushinteger(L, cell.fgID);
    lua_pushboolean(L, cell.isSolid);
    return 2;
}

static int worldSetFollow(lua_State *L)
{
    if (lua_type(L, 1) == LUA_TNIL) {
        AbelR_setFollow(NULL);
        return 0;
    }

    tAbelE_entity *e = AbelL_toEntity(L, 1);
    AbelR_setFollow(e);
    return 0;
}

static int worldGetFollow(lua_State *L)
{
    AbelL_pushEntity(L, AbelR_getFollow());
    return 1;
}

static luaL_Reg worldFunctions[] = {
    { "PosToGrid",  worldPosToGrid},
    { "GridToPos",  worldGridToPos},
    {"SetTileSet", worldSetTileSet},
    {"GetTileSet", worldGetTileSet},
    {    "Follow",  worldGetFollow},
    { "SetFollow",  worldSetFollow},
    {   "SetCell",    worldSetCell},
    {   "GetCell",    worldGetCell},
    {    "OnStep",          onStep},
    {        NULL,            NULL}
};

void AbelL_registerWorld(lua_State *L)
{
    lua_newtable(L);
    luaL_setfuncs(L, worldFunctions, 0);
    lua_setglobal(L, "World");
}