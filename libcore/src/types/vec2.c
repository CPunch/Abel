#include "types/vec2.h"

static const char *ABEL_VEC2_METATABLE = "Vec2";

static int vec2Add(lua_State *L)
{
    tAbelV_fVec2 a = AbelL_checkVec2(L, 1);
    tAbelV_fVec2 b = AbelL_checkVec2(L, 2);

    AbelL_pushVec2(L, AbelV_addfVec2(a, b));
    return 1;
}

static int vec2Sub(lua_State *L)
{
    tAbelV_fVec2 a = AbelL_checkVec2(L, 1);
    tAbelV_fVec2 b = AbelL_checkVec2(L, 2);

    AbelL_pushVec2(L, AbelV_subfVec2(a, b));
    return 1;
}

static int vec2Mul(lua_State *L)
{
    tAbelV_fVec2 a = AbelL_checkVec2(L, 1);
    tAbelV_fVec2 b = AbelL_checkVec2(L, 2);

    AbelL_pushVec2(L, AbelV_mulfVec2(a, b));
    return 1;
}

static int vec2Div(lua_State *L)
{
    tAbelV_fVec2 a = AbelL_checkVec2(L, 1);
    tAbelV_fVec2 b = AbelL_checkVec2(L, 2);

    AbelL_pushVec2(L, AbelV_divfVec2(a, b));
    return 1;
}

static int vec2Normalize(lua_State *L)
{
    tAbelV_fVec2 a = AbelL_checkVec2(L, 1);

    AbelL_pushVec2(L, AbelV_normalizefVec2(a));
    return 1;
}

static int vec2ToString(lua_State *L)
{
    tAbelV_fVec2 v = AbelL_checkVec2(L, 1);

    lua_pushfstring(L, "%s(%f, %f)", ABEL_VEC2_METATABLE, v.x, v.y);
    return 1;
}

static int vec2GetX(lua_State *L)
{
    tAbelV_fVec2 v = AbelL_checkVec2(L, 1);

    lua_pushnumber(L, v.x);
    return 1;
}

static int vec2GetY(lua_State *L)
{
    tAbelV_fVec2 v = AbelL_checkVec2(L, 1);

    lua_pushnumber(L, v.y);
    return 1;
}

static luaL_Reg vec2MetaMethods[] = {
    {     "__add",      vec2Add},
    {     "__sub",      vec2Sub},
    {     "__mul",      vec2Mul},
    {     "__div",      vec2Div},
    {"__tostring", vec2ToString},
    {        NULL,         NULL}
};

static luaL_Reg vec2Methods[] = {
    {      "Add",       vec2Add},
    {      "Sub",       vec2Sub},
    {      "Mul",       vec2Mul},
    {      "Div",       vec2Div},
    {"Normalize", vec2Normalize},
    {        "X",      vec2GetX},
    {        "Y",      vec2GetY},
    {       NULL,          NULL}
};

static int vec2New(lua_State *L)
{
    lua_Number x = luaL_checknumber(L, 1);
    lua_Number y = luaL_checknumber(L, 2);
    tAbelV_fVec2 v = AbelV_newfVec2(x, y);

    AbelL_pushVec2(L, v);
    return 1;
}

void AbelL_registerVec2(lua_State *L)
{
    luaL_newmetatable(L, ABEL_VEC2_METATABLE);
    luaL_setfuncs(L, vec2MetaMethods, 0);

    // set Vec2 methods
    lua_newtable(L);
    luaL_setfuncs(L, vec2Methods, 0);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    // set global Vec2
    lua_newtable(L);
    lua_pushcfunction(L, vec2New);
    lua_setfield(L, -2, "New");
    lua_setglobal(L, "Vec2");
}

void AbelL_pushVec2(lua_State *L, tAbelV_fVec2 v)
{
    tAbelV_fVec2 *vec = lua_newuserdata(L, sizeof(tAbelV_fVec2));
    *vec = v;
    luaL_setmetatable(L, ABEL_VEC2_METATABLE);
}

tAbelV_fVec2 AbelL_checkVec2(lua_State *L, int index)
{
    return *(tAbelV_fVec2 *)luaL_checkudata(L, index, ABEL_VEC2_METATABLE);
}