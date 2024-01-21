#ifndef ABEL_INPUT_H
#define ABEL_INPUT_H

#include "abel.h"
#include "core/event.h"

void AbelI_init(uint32_t initFlags);
void AbelI_quit(void);

tAbelVM_eventConnection *AbelI_onKeyDownConnect(tEventCallback callback, const void *uData);
tAbelVM_eventConnection *AbelI_onKeyUpConnect(tEventCallback callback, const void *uData);

tAbelVM_eventConnection **AbelI_onKeyDownHead(void);
tAbelVM_eventConnection **AbelI_onKeyUpHead(void);

void AbelI_pollEvents(void);

#endif