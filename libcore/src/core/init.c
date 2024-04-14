#include "core/init.h"

#include "assets.h"
#include "game.h"
#include "input.h"
#include "render.h"
#include "script.h"
#include "sound.h"
#include "world.h"

/* DO NOT REORGANIZE */
tAbel_initFunc initializers[] = {AbelR_init, AbelK_init, AbelI_init, AbelA_init, AbelW_init, AbelG_init, AbelL_init};

tAbel_quitFunc deinitializers[] = {AbelL_quit, AbelG_quit, AbelW_quit, AbelA_quit, AbelI_quit, AbelK_quit, AbelR_quit};

void Abel_init(uint32_t initFlags)
{
    int i;

    for (i = 0; i < (sizeof(initializers) / sizeof(tAbel_initFunc)); i++)
        initializers[i](initFlags);
}

void Abel_quit(void)
{
    int i;

    for (i = 0; i < (sizeof(deinitializers) / sizeof(tAbel_initFunc)); i++)
        deinitializers[i]();
}