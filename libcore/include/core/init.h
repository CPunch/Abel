#ifndef ABEL_INIT_H
#define ABEL_INIT_H

#include "abel.h"

#define ABEL_INIT_DEFAULT 0x00
#define ABEL_INIT_NOGUI   0x01
#define ABEL_INIT_NOAUDIO 0x02

typedef void (*tAbel_initFunc)(uint32_t);
typedef void (*tAbel_quitFunc)(void);

void Abel_init(uint32_t);
void Abel_quit(void);

#endif