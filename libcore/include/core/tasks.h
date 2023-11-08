#ifndef ABEL_TASKS_H
#define ABEL_TASKS_H

#include "abel.h"

/* while SDL's timer API is really great, it executes the callbacks on a *different* thread. This
    is a headache for us since we'll need to have proper mutexes around the codebase. This style
    of having a simple scheduler running in the main loop saves us that headache && helps keep
    things simple. */

/* return value is the next delay (if return value is 0, task will not be scheduled) */
typedef uint32_t (*taskCallback)(uint32_t, void *);

typedef struct _tAbelT_task
{
    uint32_t schedule, scheduledAt;
    void *uData;
    taskCallback callback;
    struct _tAbelT_task *next;
} tAbelT_task;

/* if delay is == 0, task will not be scheduled */
tAbelT_task *AbelT_newTask(uint32_t delay, taskCallback callback, void *uData);
void AbelT_freeTask(tAbelT_task *task);

/* utils */
void AbelT_pollTasks(void);
void AbelT_scheduleTask(tAbelT_task *task, uint32_t delay);
void AbelT_unscheduleTask(tAbelT_task *task);

#endif