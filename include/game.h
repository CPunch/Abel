#ifndef ABEL_GAME_H
#define ABEL_GAME_H

#include "abel.h"
#include "core/vec2.h"

/* initializers */
void AbelG_init(void);
void AbelG_quit(void);

tAbelV_iVec2 AbelG_getCameraPos(void);
void AbelG_setCameraPos(tAbelV_iVec2 pos);

void AbelG_run(void);

#endif