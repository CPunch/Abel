#include "types/entity.h"

#include "entity.h"
#include "types/texture.h"
#include "types/vec2.h"
#include "world.h"

static const char *ABEL_ENTITY_METATABLE = "Sprite";

static int entityGC(lua_State *L)
{
    tAbelE_entity *e = AbelL_toEntity(L, 1);
    AbelM_releaseRef(&e->refCount);
    return 0;
}

static tAbelS_sprite *toSprite(lua_State *L, int index)
{
    tAbelE_entity **ud = luaL_checkudata(L, index, ABEL_ENTITY_METATABLE);
    return &(*ud)->sprite;
}

static luaL_Reg entityMetaMethods[] = {
    {"__gc", entityGC},
    {  NULL,     NULL}
};

static int entityAdd(lua_State *L)
{
    tAbelE_entity *e = AbelL_toEntity(L, 1);

    AbelW_addEntity(e);
    return 0;
}

static int entityRemove(lua_State *L)
{
    tAbelE_entity *e = AbelL_toEntity(L, 1);

    AbelW_rmvEntity(e);
    return 0;
}

static int entityGetVelocity(lua_State *L)
{
    tAbelE_entity *e = AbelL_toEntity(L, 1);
    AbelL_pushVec2(L, AbelE_getVelocity(e));
    return 1;
}

static int entitySetVelocity(lua_State *L)
{
    tAbelE_entity *e = AbelL_toEntity(L, 1);
    tAbelV_fVec2 velocity = AbelL_checkVec2(L, 2);

    AbelE_setVelocity(e, velocity);
    return 0;
}

static int entityGetPos(lua_State *L)
{
    tAbelE_entity *e = AbelL_toEntity(L, 1);
    AbelL_pushVec2(L, AbelE_getPosition(e));
    return 1;
}

static int entitySetPos(lua_State *L)
{
    tAbelE_entity *e = AbelL_toEntity(L, 1);
    tAbelV_fVec2 position = AbelL_checkVec2(L, 2);

    AbelE_setPosition(e, position);
    return 0;
}

static int spriteGetTileSet(lua_State *L)
{
    tAbelS_sprite *s = toSprite(L, 1);
    AbelL_pushTexture(L, s->tileSet);
    return 1;
}

static int spriteAddAnimation(lua_State *L)
{
    tAbelS_sprite *s = toSprite(L, 1);

    lua_pushnumber(L, AbelS_addAnimation(s));
    return 1;
}

static int spriteAddFrame(lua_State *L)
{
    tAbelS_sprite *s = toSprite(L, 1);
    int animID = luaL_checknumber(L, 2);
    int tileID = luaL_checknumber(L, 3);
    uint32_t frameTime = luaL_checknumber(L, 4);

    AbelS_addFrame(s, animID, tileID, frameTime);
    return 0;
}

static int spritePlayAnimation(lua_State *L)
{
    tAbelS_sprite *s = toSprite(L, 1);
    int animID = luaL_checknumber(L, 2);

    AbelS_playAnimation(s, animID);
    return 0;
}

static int spriteStopAnimation(lua_State *L)
{
    tAbelS_sprite *s = toSprite(L, 1);

    AbelS_stopAnimation(s);
    return 0;
}

static luaL_Reg entityMethods[] = {
    {          "Add",           entityAdd},
    {       "Remove",        entityRemove},
    {     "Velocity",   entityGetVelocity},
    {  "SetVelocity",   entitySetVelocity},
    {          "Pos",        entityGetPos},
    {       "SetPos",        entitySetPos},
 /* sprite methods */
    {      "TileSet",    spriteGetTileSet},
    { "AddAnimation",  spriteAddAnimation},
    {     "AddFrame",      spriteAddFrame},
    {"PlayAnimation", spritePlayAnimation},
    {"StopAnimation", spriteStopAnimation},
    {           NULL,                NULL}
};

/* creates a new default entity */
static int entityNew(lua_State *L)
{
    tAbelE_entity *e = AbelM_malloc(sizeof(tAbelE_entity));
    tAbelR_texture *tileSet = AbelL_toTexture(L, 1);
    tAbelV_fVec2 pos = AbelL_checkVec2(L, 2);

    AbelE_initEntity(e, tileSet, pos, AbelE_defaultFree);
    AbelL_pushEntity(L, e);
    return 1;
}

static luaL_Reg entityFunctions[] = {
    {"New", entityNew},
    { NULL,      NULL}
};

void AbelL_registerEntity(lua_State *L)
{
    luaL_newmetatable(L, ABEL_ENTITY_METATABLE);
    luaL_setfuncs(L, entityMetaMethods, 0);

    /* set entity methods */
    lua_newtable(L);
    luaL_setfuncs(L, entityMethods, 0);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    /* register entity functions */
    lua_newtable(L);
    luaL_setfuncs(L, entityFunctions, 0);
    lua_setglobal(L, "Entity");
}

void AbelL_pushEntity(lua_State *L, tAbelE_entity *e)
{
    tAbelE_entity **ud = lua_newuserdata(L, sizeof(tAbelE_entity *));
    *ud = e;

    luaL_setmetatable(L, ABEL_ENTITY_METATABLE);
    AbelM_retainRef(&e->refCount);
}

tAbelE_entity *AbelL_toEntity(lua_State *L, int index)
{
    tAbelE_entity **ud = luaL_checkudata(L, index, ABEL_ENTITY_METATABLE);
    return *ud;
}