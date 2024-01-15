#ifndef ABEL_TYPES_ENTITY_H
#define ABEL_TYPES_ENTITY_H

#include "abel.h"
#include "entity.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

void AbelL_registerEntity(lua_State *L);

void AbelL_pushEntity(lua_State *L, tAbelE_entity *t);
tAbelE_entity *AbelL_toEntity(lua_State *L, int index);

#endif