#include "core/init.h"
#include "core/mem.h"
#include "core/vec2.h"
#include "script.h"
#include "tests.h"
#include "types/vec2.h"

static void testVec2(void)
{
    tAbelV_iVec2 v1, v2, temp;

    v1 = AbelV_newiVec2(1, 2);
    v2 = AbelV_newiVec2(3, 4);

    ABEL_TEST("AbelV_newiVec2 failed", v1.x == 1 && v1.y == 2 && v2.x == 3 && v2.y == 4);

    temp = AbelV_addiVec2(v1, v2);
    ABEL_TEST("AbelV_addiVec2 failed", temp.x == 4 && temp.y == 6);

    temp = AbelV_subiVec2(v1, v2);
    ABEL_TEST("AbelV_subiVec2 failed", temp.x == -2 && temp.y == -2);
}

static void testLuaVec2(void)
{
    Abel_init(ABEL_INIT_NOGUI | ABEL_INIT_NOAUDIO);

    tAbelVM_thread *thread = AbelL_runScript("local vec = Vec2.New(1, 2)\n"
                                             "local vec2 = Vec2.New(3, 4)\n"
                                             "local vec3 = vec:Add(vec2)\n"
                                             "print(\"result: \" .. tostring(vec3))\n"
                                             "return vec3\n",
                                             1);

    tAbelV_fVec2 vec = AbelL_checkVec2(thread->L, -1);

    ABEL_TEST("Vec2 Lua result failed", vec.x == 4 && vec.y == 6);

    AbelL_releaseThread(thread);

    Abel_quit();
}

static void testMem(void)
{
    int *ptr;

    ptr = AbelM_malloc(sizeof(int));
    ABEL_TEST("AbelM_malloc failed", ptr != NULL);

    ptr = AbelM_realloc(ptr, 0);
    ABEL_TEST("AbelM_realloc failed", ptr == NULL);
}

#define RUN_TEST(test)                                                                                                                                         \
    printf("- %s...\n", #test);                                                                                                                                \
    test();

int main(int argc, char **argv)
{
    RUN_TEST(testMem);
    RUN_TEST(testEvents);
    RUN_TEST(testVec2);
    RUN_TEST(testLuaVec2);

    printf("All tests passed!\n");
    return 0;
}