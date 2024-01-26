#ifndef ABEL_TYPES_SOUND_H
#define ABEL_TYPES_SOUND_H

#include "abel.h"
#include "sound.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

void AbelL_registerSound(lua_State *L);

void AbelL_pushSound(lua_State *L, tAbelR_sound *t);
tAbelR_sound *AbelL_toSound(lua_State *L, int index);

#endif