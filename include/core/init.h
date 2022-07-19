#ifndef ABEL_INIT_H
#define ABEL_INIT_H

#include "abel.h"

typedef void (*Abel_initFunc)(void);

void Abel_init(void);
void Abel_quit(void);

#endif