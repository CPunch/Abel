#ifndef ABEL_TYPES_TEXTURE_H
#define ABEL_TYPES_TEXTURE_H

#include "abel.h"
#include "render.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

void AbelL_registerTexture(lua_State *L);

void AbelL_pushTexture(lua_State *L, tAbelR_texture *t);
tAbelR_texture *AbelL_toTexture(lua_State *L, int index);

#endif