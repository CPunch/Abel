#include "core/tasks.h"

#include "core/mem.h"

/* task list is sorted from next in queue to last */
static tAbelT_task *head = NULL;

/* =========================================[[ Tasks ]]========================================= */

static void scheduleTask(tAbelT_task *task, uint32_t delay, uint32_t tick)
{
    tAbelT_task *curr = head, *last;
    task->scheduledAt = tick;
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

    if (task == head) {
        head = task->next;
        task->next = NULL;
        return;
    }

    /* find task in task list */
    while (curr && curr != task) {
        last = curr;
        curr = curr->next;
    }

    /* task is already unscheduled */
    if (curr == NULL) {
        task->next = NULL;
        return;
    }

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

    /* insert into linked list (if delay is non-zero) */
    if (delay)
        scheduleTask(task, delay, SDL_GetTicks());

    return task;
}

void AbelT_freeTask(tAbelT_task *task)
{
    unscheduleTask(task);
    AbelM_free(task);
}

/* returns the delay until a task is scheduled */
uint32_t AbelT_pollTasks(void)
{
    tAbelT_task *curr = head, *task;
    uint32_t currTick, nextDelay;

    /* run scheduled tasks */
    currTick = SDL_GetTicks();
    while (curr && curr->schedule < currTick) {
        task = curr;
        curr = curr->next;

        unscheduleTask(task);
        nextDelay = task->callback(currTick - task->scheduledAt, task->uData);

        /* if they returned non-zero */
        if (nextDelay)
            scheduleTask(task, nextDelay, currTick);
    }

    return curr ? curr->schedule - currTick : MAX_POLLRATE;
}

void AbelT_scheduleTask(tAbelT_task *task, uint32_t delay)
{
    unscheduleTask(task);
    scheduleTask(task, delay, SDL_GetTicks());
}

void AbelT_unscheduleTask(tAbelT_task *task)
{
    unscheduleTask(task);
}