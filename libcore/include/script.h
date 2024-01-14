#ifndef ABEL_SCRIPT_H
#define ABEL_SCRIPT_H

#include "abel.h"
#include "core/mem.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

typedef struct _tAbelVM_thread
{
    tAbelM_refCount refCount;
    lua_State *L;
    tAbelT_task *task;
    struct _tAbelVM_thread *parent;
    int status;
} tAbelVM_thread;

void AbelL_init(void);
void AbelL_quit(void);

tAbelVM_thread *AbelL_loadScript(const char *path, int *nresults);
tAbelVM_thread *AbelL_runScript(const char *script, int *nresults);
void AbelL_releaseThread(tAbelVM_thread *thread);

tAbelVM_thread *AbelL_getThread(lua_State *L);
tAbelVM_thread *AbelL_newThread(tAbelVM_thread *parent);

#endif