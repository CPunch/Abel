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

#define PLAYER_SPEED 32
#define TESTMAP_SIZE 64

typedef struct _tAbelG_state
{
    bool quit;
} tAbelG_state;

static tAbelG_state state;

/* =====================================[[ Initializers ]]====================================== */

void AbelG_init(void)
{
    int i, x, y;

    /* build pretty semi-random grass field */
    for (x = 0; x < TESTMAP_SIZE; x++) {
        for (y = 0; y < TESTMAP_SIZE; y++) {
            switch (rand() % 6) {
            case 0:
                AbelW_setCell(AbelV_newiVec2(x, y), 0, false);
                break; /* grass */
            case 1:
                AbelW_setCell(AbelV_newiVec2(x, y), 2, false);
                break; /* weed */
            default:
                AbelW_setCell(AbelV_newiVec2(x, y), 1, false);
                break; /* empty grass */
            }
        }
    }

    AbelW_setCell(AbelV_newiVec2(3, 4), 4, true);
    AbelW_setCell(AbelV_newiVec2(3, 5), 4, true);
    AbelW_setCell(AbelV_newiVec2(3, 6), 4, true);
    AbelW_setCell(AbelV_newiVec2(4, 6), 4, true);
    AbelW_setCell(AbelV_newiVec2(5, 6), 4, true);
}

void AbelG_quit(void)
{
    state.quit = true;
}

/* =====================================[[ Game Loop ]]====================================== */

void AbelG_run(void)
{
    struct nk_context *ctx = AbelR_getNuklearCtx();
    tAbelP_player *plr = AbelP_newPlayer(AbelV_newfVec2(32 * 12, 32 * 12));

    AbelW_updateActiveDistance(1);
    AbelW_addEntity(&plr->entity);
    AbelR_setFollow(&plr->entity);

    /* main engine loop */
    state.quit = false;
    while (!state.quit) {
        AbelI_pollEvents();
        SDL_Delay(AbelT_pollTasks());
    }

    AbelR_setFollow(NULL);
    AbelP_releasePlayer(plr);
}