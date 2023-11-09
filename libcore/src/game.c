#include "game.h"

#include "assets.h"
#include "chunk.h"
#include "core/tasks.h"
#include "entity.h"
#include "render.h"
#include "sprite.h"
#include "world.h"
#include "input.h"

#define PLAYER_SPEED 32
#define TESTMAP_SIZE 64

typedef struct _tAbelG_state {
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
    tAbelE_entity entity;
    struct nk_context *ctx = AbelR_getNuklearCtx();
    int animID;

    AbelE_initEntity(&entity, AbelV_i2fVec(AbelC_gridToPos(AbelV_newiVec2(0, 0))));
    animID = AbelS_addAnimation(&entity.sprite);
    for (int i = 0; i < 25; i++) {
        if (i == 15)
            continue;
        AbelS_addFrame(&entity.sprite, animID, i, 100);
    }
    AbelS_playAnimation(&entity.sprite, animID); /* play animation :D */

    AbelE_setVelocity(&entity, AbelV_newfVec2(PLAYER_SPEED, PLAYER_SPEED));
    /* main engine loop */
    state.quit = false;
    while (!state.quit) {
        AbelI_pollEvents();
        AbelT_pollTasks();

        AbelR_getCamera()->pos.x = entity.sprite.pos.x + (AbelR_tileSize.x / 2);
        AbelR_getCamera()->pos.y = entity.sprite.pos.y + (AbelR_tileSize.y / 2);
    }

    AbelE_cleanupEntity(&entity);
}