#include "types/texture.h"

#include "core/mem.h"
#include "types/vec2.h"

static const char *ABEL_TEXTURE_METATABLE = "Texture";

static int textureGC(lua_State *L)
{
    tAbelR_texture *t = AbelL_toTexture(L, 1);
    AbelM_releaseRef(&t->refCount);
    return 0;
}

static luaL_Reg textureMetaMethods[] = {
    {"__gc", textureGC},
    {  NULL,      NULL}
};

static int textureGetSize(lua_State *L)
{
    tAbelR_texture *t = AbelL_toTexture(L, 1);
    AbelL_pushVec2(L, AbelV_i2fVec(t->size));
    return 1;
}

static int textureGetTileSize(lua_State *L)
{
    tAbelR_texture *t = AbelL_toTexture(L, 1);
    AbelL_pushVec2(L, AbelV_i2fVec(t->tileSize));
    return 1;
}

static int textureSetTileSize(lua_State *L)
{
    tAbelR_texture *t = AbelL_toTexture(L, 1);
    tAbelV_iVec2 tileSize = AbelV_f2iVec(AbelL_checkVec2(L, 2));

    /* make sure size is evenly divisible by new tileSize */
    if (t->size.x % tileSize.x != 0 || t->size.y % tileSize.y != 0) {
        luaL_error(L, "Texture size must be evenly divisible by new tileSize");
    }

    t->tileSize = tileSize;
    return 0;
}

static luaL_Reg textureMethods[] = {
    {       "Size",     textureGetSize},
    {   "TileSize", textureGetTileSize},
    {"SetTileSize", textureSetTileSize},
    {         NULL,               NULL}
};

void AbelL_registerTexture(lua_State *L)
{
    luaL_newmetatable(L, ABEL_TEXTURE_METATABLE);
    luaL_setfuncs(L, textureMetaMethods, 0);

    /* set texture methods */
    lua_newtable(L);
    luaL_setfuncs(L, textureMethods, 0);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
}

void AbelL_pushTexture(lua_State *L, tAbelR_texture *t)
{
    tAbelR_texture **ud = lua_newuserdata(L, sizeof(tAbelR_texture *));
    *ud = t;

    luaL_getmetatable(L, ABEL_TEXTURE_METATABLE);
    lua_setmetatable(L, -2);
    AbelM_retainRef(&t->refCount);
}

tAbelR_texture *AbelL_toTexture(lua_State *L, int index)
{
    tAbelR_texture **ud = luaL_checkudata(L, index, ABEL_TEXTURE_METATABLE);
    return *ud;
}