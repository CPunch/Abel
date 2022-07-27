#include "core/tasks.h"

#include "core/mem.h"

/* task list is sorted from next in queue to last */
static tAbelT_task *head = NULL;

/* =========================================[[ Tasks ]]========================================= */

static void scheduleTask(tAbelT_task *task, uint32_t delay)
{
    tAbelT_task *curr = head, *last;
    task->scheduledAt = SDL_GetTicks();
    task->schedule = task->scheduledAt + delay;

    /* walk list until we find a task scheduled *prior* */
    while (curr && curr->schedule < task->schedule) {
        last = curr;
        curr = curr->next;
    }

    /* insert into list */
    if (curr == head) {
        task->next = head;
        head = task;
    } else {
        task->next = curr;
        last->next = task;
    }
}

static void unscheduleTask(tAbelT_task *task)
{
    tAbelT_task *curr = head, *last = NULL;

    /* find task in task list */
    while (curr && curr != task) {
        last = curr;
        curr = curr->next;
    }

    /* task is already unscheduled */
    if (curr == NULL)
        return;

    /* remove from list */
    if (last)
        last->next = task->next;
    task->next = NULL;
}

/* =======================================[[ Tasks API ]]======================================= */

tAbelT_task *AbelT_newTask(uint32_t delay, taskCallback callback, void *uData)
{
    tAbelT_task *task = (tAbelT_task *)AbelM_malloc(sizeof(tAbelT_task));

    task->callback = callback;
    task->uData = uData;

    /* insert into linked list */
    scheduleTask(task, delay);
}

void AbelT_freeTask(tAbelT_task *task)
{
    unscheduleTask(task);
    AbelM_free(task);
}

void AbelT_pollTasks()
{
    tAbelT_task *curr = head, *task;
    uint32_t currTick, nextDelay;

    /* run scheduled tasks */
    currTick = SDL_GetTicks();
    while (curr != NULL && curr->schedule < currTick) {
        task = curr;
        curr = curr->next;

        nextDelay = task->callback(currTick - task->scheduledAt, task->uData);
        unscheduleTask(task);

        /* if they returned non-zero */
        if (nextDelay)
            scheduleTask(task, nextDelay);
    }
}

void AbelT_scheduleTask(tAbelT_task *task, uint32_t delay)
{
    unscheduleTask(task);
    scheduleTask(task, delay);
}

void AbelT_unscheduleTask(tAbelT_task *task)
{
    unscheduleTask(task);
}