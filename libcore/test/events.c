#include "abel.h"
#include "core/event.h"
#include "tests.h"

static bool eventRan = false;

static void onEventCallback(const void *uData, const void *eventData)
{
    ABEL_TEST("Event callback failed", uData == NULL && eventData == NULL)
    eventRan = true;
}

void testEvents(void)
{
    tAbelVM_eventConnection *testEvents = NULL;

    AbelVM_connectEvent(&testEvents, onEventCallback, NULL);
    AbelVM_fireEventList(testEvents, NULL);

    ABEL_TEST("Event failed to run", eventRan);

    AbelVM_clearEventList(&testEvents);
}