#include "core/event.h"

#include "core/mem.h"

static void disconnect(tAbelVM_eventConnection *event)
{
    tAbelVM_eventConnection *curr = *event->head, *last;

    /* set the head */
    if (*event->head == event) {
        *event->head = NULL;
        return;
    }

    while (curr && curr != event) {
        last = curr;
        curr = curr->next;
    }

    /* event is already unscheduled */
    if (curr == NULL)
        return;

    /* remove from list */
    if (last)
        last->next = event->next;
}

static void freeEventConnection(tAbelM_refCount *refCount)
{
    tAbelVM_eventConnection *event = (tAbelVM_eventConnection *)refCount;

    printf("freeEventConnection %p\n", event);
    /* remove from list */
    disconnect(event);
    AbelM_free(event);
}

tAbelVM_eventConnection *AbelVM_connectEvent(tAbelVM_eventConnection **head, tEventCallback callback, const void *uData)
{
    tAbelVM_eventConnection *conn = (tAbelVM_eventConnection *)AbelM_malloc(sizeof(tAbelVM_eventConnection));
    conn->callback = callback;
    conn->uData = uData;
    conn->next = *head;
    conn->head = head;
    AbelM_initRef(&conn->refCount, freeEventConnection);

    *head = conn;
    return conn;
}

void AbelVM_disconnectEvent(tAbelVM_eventConnection *event)
{
    disconnect(event);
    AbelM_releaseRef(&event->refCount);
}

void AbelVM_clearEventList(tAbelVM_eventConnection **head)
{
    tAbelVM_eventConnection *curr = *head, *next;

    while (curr) {
        next = curr->next;
        AbelM_releaseRef(&curr->refCount);
        curr = next;
    }

    *head = NULL;
}

void AbelVM_fireEventList(tAbelVM_eventConnection *event, const void *eventData)
{
    tAbelVM_eventConnection *curr = event, *next;

    while (curr) {
        next = curr->next;
        curr->callback(curr->uData, eventData);
        curr = next;
    }
}