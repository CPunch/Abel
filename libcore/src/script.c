#include "script.h"

#include "abel.h"
#include "core/mem.h"
#include "core/tasks.h"

typedef struct
{
    lua_State *L;
    int threadLookupRef;
} tAbelVM_state;

static tAbelVM_state state = {0};

static void freeThread(tAbelVM_thread *thread);
static void freeThreadWrapper(tAbelM_refCount *refCount);

/* returns next delay (if 0, task will not be scheduled) */
static int runThread(tAbelVM_thread *thread, int args)
{
    int nresults;
    lua_State *L = thread->L;

    switch (thread->status = lua_resume(L, state.L, args, &nresults)) {
    case LUA_OK:
        break;
    case LUA_YIELD:
        /* ??? */
        if (nresults != 1) {
            luaL_error(L, "Invalid number of results from yield");
            break;
        }

        /* thread is suspended, schedule it */
        uint32_t delay = lua_tointeger(L, -1);
        lua_pop(L, 1);
        return delay;
    default:
        /* oh no, error !*/
        luaL_error(L, "%s", lua_tostring(L, -1));
        break;
    }

    /* thread is finished, release our reference */
    AbelL_releaseThread(thread);
    return 0;
}

static uint32_t wakeupTask(uint32_t delta, void *data)
{
    tAbelVM_thread *thread = data;
    lua_State *L = thread->L;

    lua_pushnumber(L, delta / 1000.f);
    return runThread(thread, 1);
}

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

static tAbelVM_thread *newThread(lua_State *L)
{
    tAbelVM_thread *thread = AbelM_malloc(sizeof(tAbelVM_thread));
    AbelM_initRef(&thread->refCount, freeThreadWrapper);
    thread->L = L;
    thread->task = AbelT_newTask(0, wakeupTask, thread);
    thread->parent = NULL;

    /* add to threadLookup */
    setThreadLookup(L, thread);
    return thread;
}

static void freeThread(tAbelVM_thread *thread)
{
    /* remove from threadLookup */
    if (state.L) {
        removeThreadLookup(thread->L);
        lua_closethread(thread->L, state.L);
    }

    thread->L = NULL;

    /* release parent */
    if (thread->parent) {
        AbelM_releaseRef(&thread->parent->refCount);
    }

    AbelT_freeTask(thread->task);
    AbelM_free(thread);
}

static void freeThreadWrapper(tAbelM_refCount *refCount)
{
    freeThread((tAbelVM_thread *)refCount);
}

static int l_wait(lua_State *L)
{
    lua_Number seconds = luaL_checknumber(L, 1);
    lua_pushnumber(L, seconds * 1000);

    printf("yielding thread %p for %f seconds\n", AbelL_getThread(L), seconds);
    return lua_yield(L, 1);
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
    lua_register(state.L, "wait", l_wait);
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

tAbelVM_thread *AbelL_startScript(const char *path)
{
    lua_State *L = lua_newthread(state.L);
    tAbelVM_thread *thread = newThread(L);
    AbelM_retainRef(&thread->refCount);

    /* load script */
    luaL_loadfile(L, path);
    AbelT_scheduleTask(thread->task, runThread(thread, 0));
    lua_pop(L, 1);
    return thread;
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

/* creates a new thread */
tAbelVM_thread *AbelL_newThread(tAbelVM_thread *parent)
{
    lua_State *L = lua_newthread(parent->L);
    tAbelVM_thread *thread = newThread(L);

    thread->parent = parent;
    AbelM_retainRef(&parent->refCount);
    lua_pop(L, 1); /* pop lua thread */
    return thread;
}