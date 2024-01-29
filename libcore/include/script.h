#ifndef ABEL_SCRIPT_H
#define ABEL_SCRIPT_H

#include "abel.h"
#include "core/event.h"
#include "core/mem.h"
#include "core/tasks.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

typedef struct
{
    tAbelT_task *task;
    tAbelVM_thread *thread;
    int callbackRef;
} tAbelVM_luaTask;

typedef struct
{
    tAbelM_refCount refCount;
    tAbelVM_eventConnection *event;
    tAbelVM_thread *thread;
    tEventCallback callback;
    const void *uData;
    int callbackRef;
} tAbelVM_luaEvent;

typedef struct _tAbelVM_thread
{
    tAbelM_refCount refCount;
    lua_State *L;
    int status;
    AbelM_newVector(tAbelVM_luaTask *, runningTasks);
    AbelM_newVector(tAbelVM_luaEvent *, events);
} tAbelVM_thread;

void AbelL_init(uint32_t initFlags);
void AbelL_quit(void);

lua_State *AbelL_globalState(void);

tAbelVM_thread *AbelL_loadScript(const char *path, int nresults);
tAbelVM_thread *AbelL_runScript(const char *script, int nresults);
void AbelL_releaseThread(tAbelVM_thread *thread);

bool AbelL_callFunction(tAbelVM_thread *thread, int nargs, int nresults);

/* expects lua_function at the top of the stack; pushes event */
void AbelL_connectEvent(tAbelVM_thread *thread, tAbelVM_eventConnection **event, tEventCallback callback, const void *uData);

tAbelVM_thread *AbelL_getThread(lua_State *L);

#endif