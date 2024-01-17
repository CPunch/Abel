#include "script.h"

#include "abel.h"
#include "core/event.h"
#include "core/mem.h"
#include "core/tasks.h"
#include "types/entity.h"
#include "types/texture.h"
#include "types/vec2.h"
#include "types/world.h"

typedef struct
{
    lua_State *L;
    int threadLookupRef;
} tAbelVM_state;

static tAbelVM_state state = {0};

static bool callThread(tAbelVM_thread *thread, int args, int nresults);

/* =======================================[[ Lua Tasks ]]========================================= */

static void removeLuaTask(tAbelVM_luaTask *userData)
{
    tAbelVM_thread *thread = userData->thread;

    /* search for task and remove */
    for (int i = 0; i < AbelM_countVector(thread->runningTasks); i++) {
        if (thread->runningTasks[i] == userData) {
            AbelM_rmvVector(thread->runningTasks, i, 1);
            userData->thread = NULL;
            return;
        }
    }
}

static void freeLuaTask(tAbelVM_luaTask *userData)
{
    printf("freeing task\n");
    removeLuaTask(userData);
    AbelT_freeTask(userData->task);
    AbelM_free(userData);
}

static uint32_t wakeupLuaTask(uint32_t delta, void *data)
{
    tAbelVM_luaTask *userData = data;
    tAbelVM_thread *thread = userData->thread;
    lua_State *L = thread->L;
    uint32_t delay = 0;

    /* push callback */
    lua_rawgeti(L, LUA_REGISTRYINDEX, userData->callbackRef);
    lua_pushnumber(L, (lua_Number)delta);

    /* if error, release task */
    if (!callThread(thread, 1, 1)) {
        goto _wakeupTask_fail;
    }

    /* get delay result */
    delay = (uint32_t)lua_tonumber(L, -1);
    if (delay) {
        return delay;
    }

_wakeupTask_fail:
    /* free task */
    freeLuaTask(userData);
    return 0;
}

static int startTask(lua_State *L)
{
    tAbelVM_thread *thread = AbelL_getThread(L);
    tAbelVM_luaTask *userData = AbelM_malloc(sizeof(tAbelVM_luaTask));
    userData->thread = thread;

    /* get callback */
    luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_pushvalue(L, 1);
    userData->callbackRef = luaL_ref(L, LUA_REGISTRYINDEX);

    /* get delay */
    uint32_t delay = (uint32_t)luaL_checknumber(L, 2);
    userData->task = AbelT_newTask(delay, wakeupLuaTask, userData);

    AbelM_pushVector(tAbelVM_luaTask *, thread->runningTasks, userData);
    return 0;
}

/* =======================================[[ Thread ]]========================================= */

static void setThreadLookup(lua_State *L, tAbelVM_thread *thread)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, state.threadLookupRef);
    lua_pushthread(L);                /* key: lua thread */
    lua_pushlightuserdata(L, thread); /* value: thread */
    lua_rawset(L, -3);                /* threadLookup[lua thread] = thread */
    lua_pop(L, 1);                    /* pop threadLookup */
}

static void removeThreadLookup(lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, state.threadLookupRef);
    lua_pushthread(L); /* key: lua thread */
    lua_pushnil(L);    /* value: nil */
    lua_rawset(L, -3); /* threadLookup[lua thread] = nil */
    lua_pop(L, 1);     /* pop threadLookup */
}

static void freeThread(tAbelVM_thread *thread)
{
    /* free tasks */
    for (int i = 0; i < AbelM_countVector(thread->runningTasks); i++) {
        freeLuaTask(thread->runningTasks[i]);
    }

    /* free events */
    for (int i = 0; i < AbelM_countVector(thread->events); i++) {
        AbelVM_disconnectEvent(thread->events[i]);
    }

    /* remove from threadLookup */
    if (state.L) {
        removeThreadLookup(thread->L);
        lua_closethread(thread->L, state.L);
    }

    thread->L = NULL;
    AbelM_free(thread->runningTasks);
    AbelM_free(thread->events);
    AbelM_free(thread);
    printf("thread freed\n");
}

static void freeThreadWrapper(tAbelM_refCount *refCount)
{
    freeThread((tAbelVM_thread *)refCount);
}

static tAbelVM_thread *newThread(lua_State *L)
{
    tAbelVM_thread *thread = AbelM_malloc(sizeof(tAbelVM_thread));
    AbelM_initRef(&thread->refCount, freeThreadWrapper);
    AbelM_initVector(thread->runningTasks, 4);
    AbelM_initVector(thread->events, 4);
    thread->L = L;

    /* add to threadLookup */
    setThreadLookup(L, thread);
    return thread;
}

static bool callThread(tAbelVM_thread *thread, int args, int nresults)
{
    lua_State *L = thread->L;

    if ((thread->status = lua_pcall(L, args, nresults, 0)) != LUA_OK) {
        /* oh no, error !*/
        printf("Lua error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return false;
    }

    return true;
}

/* =======================================[[ Events ]]=========================================== */

static const char *ABEL_EVENT_METATABLE = "Event";

static void removeLuaEvent(tAbelVM_luaEvent *userData)
{
    tAbelVM_thread *thread = userData->thread;

    /* search for event and remove */
    for (int i = 0; i < AbelM_countVector(thread->events); i++) {
        if (thread->events[i] == userData->event) {
            AbelM_rmvVector(thread->events, i, 1);
            return;
        }
    }
}

static tAbelVM_luaEvent *toEventUserData(lua_State *L, int index)
{
    return (tAbelVM_luaEvent *)luaL_checkudata(L, index, ABEL_EVENT_METATABLE);
}

static int eventDisconnect(lua_State *L)
{
    tAbelVM_luaEvent *userData = toEventUserData(L, 1);

    if (userData->event) {
        removeLuaEvent(userData);
        AbelVM_disconnectEvent(userData->event);
    }

    userData->event = NULL;
    return 0;
}

static luaL_Reg eventMetaMethods[] = {
    {NULL, NULL}
};

static luaL_Reg eventMethods[] = {
    {"disconnect", eventDisconnect},
    {        NULL,            NULL}
};

void AbelL_registerEvent(lua_State *L)
{
    luaL_newmetatable(L, ABEL_EVENT_METATABLE);
    luaL_setfuncs(L, eventMetaMethods, 0);

    lua_newtable(L);
    luaL_setfuncs(L, eventMethods, 0);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
}

static void eventCallback(const void *uData, const void *eventData)
{
    tAbelVM_luaEvent *userData = (tAbelVM_luaEvent *)uData;
    tAbelVM_thread *thread = userData->thread;

    /* push lua callback */
    lua_rawgeti(thread->L, LUA_REGISTRYINDEX, userData->callbackRef);

    /* call original c callback */
    userData->callback(userData, eventData);
}

void AbelL_connectEvent(tAbelVM_thread *thread, tAbelVM_eventConnection **event, tEventCallback callback, const void *uData)
{
    lua_State *L = thread->L;
    luaL_checktype(L, 1, LUA_TFUNCTION);

    /* register function */
    int callbackRef = luaL_ref(L, LUA_REGISTRYINDEX);

    /* create event */
    tAbelVM_luaEvent *userData = lua_newuserdata(L, sizeof(tAbelVM_luaEvent));
    userData->uData = uData;
    userData->callbackRef = callbackRef;
    userData->callback = callback;
    userData->event = AbelVM_connectEvent(event, eventCallback, (const void *)userData);
    userData->thread = thread;
    luaL_setmetatable(L, ABEL_EVENT_METATABLE);

    AbelM_pushVector(tAbelVM_eventConnection *, userData->thread->events, userData->event);
}

/* =======================================[[ Script API ]]======================================= */

void AbelL_init(void)
{
    state.L = luaL_newstate();
    luaL_openlibs(state.L);

    /* register state lookup table */
    lua_newtable(state.L);
    state.threadLookupRef = luaL_ref(state.L, LUA_REGISTRYINDEX);

    /* register API */
    lua_pushcfunction(state.L, startTask);
    lua_setglobal(state.L, "startTask");

    AbelL_registerEvent(state.L);
    AbelL_registerVec2(state.L);
    AbelL_registerTexture(state.L);
    AbelL_registerEntity(state.L);
    AbelL_registerWorld(state.L);
}

void AbelL_quit(void)
{
    /* release all yielded threads */
    lua_rawgeti(state.L, LUA_REGISTRYINDEX, state.threadLookupRef);
    lua_pushnil(state.L);
    while (lua_next(state.L, -2)) {
        tAbelVM_thread *thread = lua_touserdata(state.L, -1);
        if (thread->status == LUA_YIELD) {
            AbelL_releaseThread(thread);
        }

        lua_pop(state.L, 1);
    }
    lua_pop(state.L, 1);

    luaL_unref(state.L, LUA_REGISTRYINDEX, state.threadLookupRef);

    lua_close(state.L);
    state.L = NULL;
}

tAbelVM_thread *AbelL_loadScript(const char *path, int nresults)
{
    lua_State *L = lua_newthread(state.L);
    tAbelVM_thread *thread = newThread(L);
    lua_pop(state.L, 1);

    /* load script */
    luaL_loadfile(L, path);
    if (!callThread(thread, 0, nresults))
        return NULL;

    return thread;
}

tAbelVM_thread *AbelL_runScript(const char *script, int nresults)
{
    lua_State *L = lua_newthread(state.L);
    tAbelVM_thread *thread = newThread(L);
    lua_pop(state.L, 1);

    /* load script */
    luaL_loadstring(L, script);
    if (!callThread(thread, 0, nresults))
        return NULL;

    return thread;
}

bool AbelL_callFunction(tAbelVM_thread *thread, int nargs, int nresults)
{
    return callThread(thread, nargs, nresults);
}

void AbelL_releaseThread(tAbelVM_thread *thread)
{
    AbelM_releaseRef(&thread->refCount);
}

tAbelVM_thread *AbelL_getThread(lua_State *L)
{
    tAbelVM_thread *thread;

    /* get threadLookup */
    lua_rawgeti(L, LUA_REGISTRYINDEX, state.threadLookupRef);
    lua_pushthread(L); /* key: lua thread */
    lua_rawget(L, -2); /* value: thread */
    thread = lua_touserdata(L, -1);
    lua_pop(L, 2); /* pop threadLookup and thread */

    ABEL_ASSERT(thread != NULL);
    return thread;
}
