#ifndef ABEL_INPUT_H
#define ABEL_INPUT_H

#include "abel.h"
#include "core/event.h"

void AbelI_init(void);
void AbelI_quit(void);

void AbelI_onKeyDownConnect(tEventCallback callback, const void *uData);
void AbelI_onKeyUpConnect(tEventCallback callback, const void *uData);

void AbelI_pollEvents(void);

#endif