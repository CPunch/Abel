#include "types/texture.h"

#include "assets.h"
#include "core/mem.h"
#include "types/vec2.h"

static const char *ABEL_TEXTURE_METATABLE = "Texture";

#define CHECK_SIZE(size, tileSize)                                                                                                                             \
    if (size.x % tileSize.x != 0 || size.y % tileSize.y != 0) {                                                                                                \
        luaL_error(L, "Texture size must be evenly divisible by tileSize");                                                                                    \
    }

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

    /* sanity check tileset size */
    CHECK_SIZE(t->size, t->tileSize);

    t->tileSize = tileSize;
    return 0;
}

static luaL_Reg textureMethods[] = {
    {       "Size",     textureGetSize},
    {   "TileSize", textureGetTileSize},
    {"SetTileSize", textureSetTileSize},
    {         NULL,               NULL}
};

static int textureLoad(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);
    tAbelV_iVec2 tileSize = AbelV_f2iVec(AbelL_checkVec2(L, 2));
    tAbelR_texture *t = AbelA_getTexture(path, tileSize);

    /* sanity check tileset size */
    CHECK_SIZE(t->size, t->tileSize);

    AbelL_pushTexture(L, t);
    return 1;
}

static luaL_Reg textureFunctions[] = {
    {"Load", textureLoad},
    {  NULL,        NULL}
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

    /* register texture functions */
    lua_newtable(L);
    luaL_setfuncs(L, textureFunctions, 0);
    lua_setglobal(L, "Texture");
}

void AbelL_pushTexture(lua_State *L, tAbelR_texture *t)
{
    tAbelR_texture **ud = lua_newuserdata(L, sizeof(tAbelR_texture *));
    *ud = t;

    luaL_setmetatable(L, ABEL_TEXTURE_METATABLE);
    AbelM_retainRef(&t->refCount);
}

tAbelR_texture *AbelL_toTexture(lua_State *L, int index)
{
    tAbelR_texture **ud = luaL_checkudata(L, index, ABEL_TEXTURE_METATABLE);
    return *ud;
}