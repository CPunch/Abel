#ifndef ABEL_INPUT_H
#define ABEL_INPUT_H

#include "abel.h"
#include "core/event.h"

void AbelI_init(void);
void AbelI_quit(void);

tAbelVM_eventConnection *AbelI_onKeyDownConnect(tEventCallback callback, const void *uData);
void AbelI_onKeyDownDisconnect(tAbelVM_eventConnection *event);
tAbelVM_eventConnection *AbelI_onKeyUpConnect(tEventCallback callback, const void *uData);
void AbelI_onKeyUpDisconnect(tAbelVM_eventConnection *event);

void AbelI_pollEvents(void);

#endif