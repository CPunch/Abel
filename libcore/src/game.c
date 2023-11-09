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
    int i, animID;
    uint32_t lastTick, currTick = SDL_GetTicks();

    AbelE_initEntity(&entity, AbelV_i2fVec(AbelC_gridToPos(AbelV_newiVec2(0, 0))));
    animID = AbelS_addAnimation(&entity.sprite);
    for (int i = 0; i < 25; i++) {
        if (i == 15)
            continue;
        AbelS_addFrame(&entity.sprite, animID, i, 100);
    }
    AbelS_playAnimation(&entity.sprite, animID); /* play animation :D */

    /* main engine loop */
    state.quit = false;
    while (!state.quit) {
        AbelI_pollEvents();
        AbelT_pollTasks();

        AbelR_getCamera()->pos.x = entity.sprite.pos.x + (AbelR_tileSize.x / 2);
        AbelR_getCamera()->pos.y = entity.sprite.pos.y + (AbelR_tileSize.y / 2);

        /* windowless watermark for realtime stats */
        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_hide());
        if (nk_begin(ctx, "DEBUG", nk_rect(0, 0, 210, 120), NK_WINDOW_NO_SCROLLBAR)) {
            nk_layout_row_static(ctx, 13, 150, 1);
            nk_labelf(ctx, NK_TEXT_LEFT, "ABEL v0.1");
            nk_layout_row_static(ctx, 13, 200, 1);
            nk_labelf(ctx, NK_TEXT_LEFT, "FPS: %d", AbelW_getFPS());
            nk_labelf(ctx, NK_TEXT_LEFT, "FT (MS): %d", currTick - lastTick);
        }
        nk_end(ctx);
        nk_style_pop_style_item(ctx);

        AbelW_render();
        lastTick = currTick;
        currTick = SDL_GetTicks();
    }

    AbelE_cleanupEntity(&entity);
}