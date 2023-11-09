#include "core/event.h"

#include "core/mem.h"

void AbelVM_connectEvent(tAbelVM_eventConnection **head, tEventCallback callback, const void *uData)
{
    tAbelVM_eventConnection *conn = (tAbelVM_eventConnection *)AbelM_malloc(sizeof(tAbelVM_eventConnection));
    conn->callback = callback;
    conn->uData = uData;
    conn->next = *head;

    *head = conn;
}

void AbelVM_disconnectEvent(tAbelVM_eventConnection **head, tAbelVM_eventConnection *event)
{
    tAbelVM_eventConnection *curr = *head, *last;

    /* set the head */
    if (*head == event) {
        *head = NULL;
        goto _freeEvent;
    }

    while (curr && curr != event) {
        last = curr;
        curr = curr->next;
    }

    /* event is already unscheduled */
    if (curr == NULL)
        goto _freeEvent;

    /* remove from list */
    if (last)
        last->next = event->next;

_freeEvent:
    AbelM_free(event);
}

void AbelVM_clearEventList(tAbelVM_eventConnection **head)
{
    tAbelVM_eventConnection *curr = *head, *next;

    while (curr) {
        next = curr->next;
        AbelM_free(curr);
        curr = next;
    }

    *head = NULL;
}

void AbelVM_fireEventList(tAbelVM_eventConnection *event, const void *eventData)
{
    tAbelVM_eventConnection *curr = event;

    while (curr) {
        curr->callback(curr->uData, eventData);
        curr = curr->next;
    }
}