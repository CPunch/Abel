#ifndef ABEL_TYPES_VEC2_H
#define ABEL_TYPES_VEC2_H

#include "abel.h"
#include "core/vec2.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

/*
  our vec2 struct is defined in core/vec2.h

  create a new Vec2 with:
  local vec = Vec2.New(x, y)

  add two Vec2s together with:
  local vec = Vec2.Add(a, b)
  - or -
  local vec = a:Add(b)
  - or -
  local vec = a + b

  subtract two Vec2s with:
  local vec = Vec2.Sub(a, b)
  - or -
  local vec = a:Sub(b)
  - or -
  local vec = a - b

  multiply two Vec2s with:
  local vec = Vec2.Mul(a, b)
  - or -
  local vec = a:Mul(b)
  - or -
  local vec = a * b

  divide two Vec2s with:
  local vec = Vec2.Div(a, b)
  - or -
  local vec = a:Div(b)
  - or -
  local vec = a / b

  normalize a Vec2 with:
  local vec = Vec2.Normalize(a)
  - or -
  local vec = a:Normalize()

  read the X and Y components of a Vec2 with:
  local x = Vec2.X(a)
  local y = Vec2.Y(a)
  - or -
  local x = a:X()
  local y = a:Y()
*/

void AbelL_registerVec2(lua_State *L);

void AbelL_pushVec2(lua_State *L, tAbelV_fVec2 v);
tAbelV_fVec2 AbelL_checkVec2(lua_State *L, int index);

#endif