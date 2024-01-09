#include "core/init.h"

#include "assets.h"
#include "game.h"
#include "input.h"
#include "render.h"
#include "script.h"
#include "world.h"

/* DO NOT REORGANIZE, in order for what needs to be initialized first */
Abel_initFunc initializers[] = {AbelR_init, AbelI_init, AbelA_init, AbelW_init, AbelG_init, AbelL_init};
Abel_initFunc deinitializers[] = {AbelL_quit, AbelG_quit, AbelW_quit, AbelA_quit, AbelI_quit, AbelR_quit};

void Abel_init(void)
{
    int i;

    for (i = 0; i < (sizeof(initializers) / sizeof(Abel_initFunc)); i++)
        initializers[i]();
}

void Abel_quit(void)
{
    int i;

    for (i = 0; i < (sizeof(deinitializers) / sizeof(Abel_initFunc)); i++)
        deinitializers[i]();
}