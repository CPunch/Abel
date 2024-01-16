#ifndef ABEL_SCRIPT_H
#define ABEL_SCRIPT_H

#include "abel.h"
#include "core/mem.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

typedef struct
{
    tAbelT_task *task;
    tAbelVM_thread *thread;
    int callbackRef;
} tAbelVM_luaTask;

typedef struct _tAbelVM_thread
{
    tAbelM_refCount refCount;
    lua_State *L;
    int status;
    AbelM_newVector(tAbelVM_luaTask *, runningTasks);
} tAbelVM_thread;

void AbelL_init(void);
void AbelL_quit(void);

tAbelVM_thread *AbelL_loadScript(const char *path, int nresults);
tAbelVM_thread *AbelL_runScript(const char *script, int nresults);
bool AbelL_callFunction(tAbelVM_thread *thread, int nargs, int nresults);
void AbelL_releaseThread(tAbelVM_thread *thread);

tAbelVM_thread *AbelL_getThread(lua_State *L);

#endif