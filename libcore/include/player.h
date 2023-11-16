#ifndef ABEL_PLAYER_H
#define ABEL_PLAYER_H

#include "abel.h"
#include "core/vec2.h"
#include "entity.h"

/* player is refcounted */
typedef struct _tAbelP_player
{
    tAbelE_entity entity;
    tAbelVM_eventConnection *onKeyDown;
    tAbelVM_eventConnection *onKeyUp;
    tAbelVM_eventConnection *onStep;
    int idleAnim;
    int upAnim;
    int downAnim;
    int leftAnim;
    int rightAnim;
    int upLeftAnim;
    int upRightAnim;
    int downLeftAnim;
    int downRightAnim;
    int walkSpeed;
    int selectedYDir;
    int selectedXDir;
} tAbelP_player;

/* returned player is already retained */
tAbelP_player *AbelP_newPlayer(tAbelV_fVec2 pos);
void AbelP_retainPlayer(tAbelP_player *player);
void AbelP_releasePlayer(tAbelP_player *player);

#endif