#include "game.h"

#include "assets.h"
#include "chunk.h"
#include "core/tasks.h"
#include "entity.h"
#include "input.h"
#include "player.h"
#include "render.h"
#include "sprite.h"
#include "world.h"

typedef struct _tAbelG_state
{
    bool quit;
} tAbelG_state;

static tAbelG_state state;

/* =====================================[[ Initializers ]]====================================== */

void AbelG_init(uint32_t initFlags)
{
    int i, x, y;
}

void AbelG_quit(void)
{
    state.quit = true;
}

/* =====================================[[ Game Loop ]]====================================== */

/* main engine loop */
void AbelG_run(void)
{
    state.quit = false;
    while (!state.quit) {
        AbelI_pollEvents();
        SDL_Delay(AbelT_pollTasks());
    }
}