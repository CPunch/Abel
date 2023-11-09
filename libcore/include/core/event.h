#ifndef ABEL_EVENT_H
#define ABEL_EVENT_H

typedef void (*tEventCallback)(const void *uData, const void *eventData);

typedef struct _tAbelVM_eventConnection
{
    tEventCallback callback;
    const void *uData;
    struct _tAbelVM_eventConnection *next;
} tAbelVM_eventConnection;

void AbelVM_connectEvent(tAbelVM_eventConnection **head, tEventCallback callback, const void *uData);
void AbelVM_disconnectEvent(tAbelVM_eventConnection **head, tAbelVM_eventConnection *event);

void AbelVM_clearEventList(tAbelVM_eventConnection **head);
void AbelVM_fireEventList(tAbelVM_eventConnection *event, const void *eventData);

#endif