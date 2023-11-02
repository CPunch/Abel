#include "game.h"

#include "assets.h"
#include "chunk.h"
#include "core/tasks.h"
#include "entity.h"
#include "map.h"
#include "render.h"
#include "sprite.h"

#define PLAYER_SPEED 32
#define TESTMAP_SIZE 64

/* =====================================[[ Initializers ]]====================================== */

void AbelG_init(void)
{
    int i, x, y;

    /* build pretty semi-random grass field */
    for (x = 0; x < TESTMAP_SIZE; x++) {
        for (y = 0; y < TESTMAP_SIZE; y++) {
            switch (rand() % 6) {
            case 0:
                AbelM_setCell(AbelV_newiVec2(x, y), 0, false);
                break; /* grass */
            case 1:
                AbelM_setCell(AbelV_newiVec2(x, y), 2, false);
                break; /* weed */
            default:
                AbelM_setCell(AbelV_newiVec2(x, y), 1, false);
                break; /* empty grass */
            }
        }
    }
}

void AbelG_quit(void)
{
}

/* =====================================[[ Game Loop ]]====================================== */

void AbelG_run(void)
{
    SDL_Event evnt;
    tAbelE_entity *entity;
    tAbelR_texture *tileset;
    struct nk_context *ctx = AbelR_getNuklearCtx();
    int i, animID;
    bool quit = false;

    tileset = AbelA_getTexture(ASSET_ID_SPRITE_TILESET);
    entity = AbelE_newEntity(tileset, AbelV_i2fVec(AbelC_gridToPos(AbelV_newiVec2(0, 0))));
    animID = AbelS_addAnimation(entity->sprite);
    AbelS_addFrame(entity->sprite, animID, 16, 1000); /* tile id 16 for 1 second */
    AbelS_addFrame(entity->sprite, animID, 17, 100);  /* tile id 17 for .1 seconds */
    AbelS_playAnimation(entity->sprite, animID);      /* play animation :D */

    AbelM_setCell(AbelV_newiVec2(3, 4), 4, true);
    AbelM_setCell(AbelV_newiVec2(3, 5), 4, true);
    AbelM_setCell(AbelV_newiVec2(3, 6), 4, true);
    AbelM_setCell(AbelV_newiVec2(4, 6), 4, true);
    AbelM_setCell(AbelV_newiVec2(5, 6), 4, true);

    /* main engine loop */
    while (!quit) {
        /* handle SDL events */
        nk_input_begin(ctx);
        while (SDL_PollEvent(&evnt) != 0) {
            switch (evnt.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch (evnt.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_w:
                    AbelE_setVelocity(entity, AbelV_newfVec2(entity->velocity.x, -PLAYER_SPEED));
                    break;
                case SDLK_s:
                    AbelE_setVelocity(entity, AbelV_newfVec2(entity->velocity.x, PLAYER_SPEED));
                    break;
                case SDLK_a:
                    AbelE_setVelocity(entity, AbelV_newfVec2(-PLAYER_SPEED, entity->velocity.y));
                    break;
                case SDLK_d:
                    AbelE_setVelocity(entity, AbelV_newfVec2(PLAYER_SPEED, entity->velocity.y));
                    break;
                default:
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (evnt.key.keysym.sym) {
                case SDLK_w:
                    AbelE_setVelocity(entity, AbelV_newfVec2(entity->velocity.x, 0));
                    break;
                case SDLK_s:
                    AbelE_setVelocity(entity, AbelV_newfVec2(entity->velocity.x, 0));
                    break;
                case SDLK_a:
                    AbelE_setVelocity(entity, AbelV_newfVec2(0, entity->velocity.y));
                    break;
                case SDLK_d:
                    AbelE_setVelocity(entity, AbelV_newfVec2(0, entity->velocity.y));
                    break;
                default:
                    break;
                }
                break;
            /* handle mouse zoom in/out */
            case SDL_MOUSEWHEEL:
                if (evnt.wheel.y > 0) {
                    AbelR_zoomCamera(1);
                } else if (evnt.wheel.y < 0) {
                    AbelR_zoomCamera(-1);
                }
                break;
            default:
                break;
            }
            nk_sdl_handle_event(&evnt);
        }
        nk_input_end(ctx);

        /* run scheduled tasks */
        AbelT_pollTasks();

        AbelR_getCamera()->pos.x = entity->sprite->pos.x + (AbelR_tileSize.x / 2);
        AbelR_getCamera()->pos.y = entity->sprite->pos.y + (AbelR_tileSize.y / 2);

        /* windowless watermark for realtime stats
           TODO: add realtime stats lol */
        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_hide());
        if (nk_begin(ctx, "DEBUG", nk_rect(0, 0, 210, 120), NK_WINDOW_NO_SCROLLBAR)) {
            nk_layout_row_static(ctx, 13, 150, 1);
            nk_labelf(ctx, NK_TEXT_LEFT, "ABEL v0.1");
        }
        nk_end(ctx);
        nk_style_pop_style_item(ctx);

        /* clear layers */
        SDL_RenderClear(AbelR_getRenderer());

        /* render chunks */
        AbelM_renderChunks(LAYER_BG);
        AbelM_renderEntities();
        nk_sdl_render(NK_ANTI_ALIASING_ON);

        /* render to window */
        SDL_RenderPresent(AbelR_getRenderer());
    }

    AbelE_freeEntity(entity);
}