#ifndef ABEL_PLAYER_H
#define ABEL_PLAYER_H

#include "abel.h"
#include "core/vec2.h"
#include "entity.h"

typedef struct _tAbelP_player {
  tAbelE_entity entity;
  int walkSpeed;
  int selectedYDir;
  int selectedXDir;
} tAbelP_player;

tAbelP_player *AbelP_newPlayer(tAbelV_iVec2 pos);
void AbelP_freePlayer(tAbelP_player *player);

#endif