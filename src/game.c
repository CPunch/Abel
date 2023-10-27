#include "game.h"

#include "assets.h"
#include "chunk.h"
#include "core/tasks.h"
#include "entity.h"
#include "map.h"
#include "render.h"
#include "sprite.h"

ASSET_ID tilesetID;
ASSET_ID debugFontID;

#define PLAYER_SPEED 32
#define TESTMAP_SIZE 64

/* =====================================[[ Initializers ]]====================================== */

void AbelG_init(void)
{
    int i, x, y;
    tAbelR_texture *tileset;

    /* TODO: load this data from game file */
    tilesetID = AbelA_loadAsset("res/tileset.png", ASSET_TEXTURE);
    debugFontID = AbelA_loadAsset("res/kongtext.ttf", ASSET_FONT);

    tileset = AbelA_getTexture(tilesetID);

    /* build pretty semi-random grass field */
    for (x = 0; x < TESTMAP_SIZE; x++) {
        for (y = 0; y < TESTMAP_SIZE; y++) {
            switch (rand() % 6) {
            case 0:
                AbelM_setCell(AbelV_newiVec2(x, y), tileset, 0, false);
                break; /* grass */
            case 1:
                AbelM_setCell(AbelV_newiVec2(x, y), tileset, 2, false);
                break; /* weed */
            default:
                AbelM_setCell(AbelV_newiVec2(x, y), tileset, 1, false);
                break; /* empty grass */
            }
        }
    }
}

void AbelG_quit(void)
{
    int i;

    AbelA_freeAsset(tilesetID);
    AbelA_freeAsset(debugFontID);
}

void AbelG_run(void)
{
    SDL_Event evnt;
    tAbelE_entity *entity;
    tAbelR_texture *tileset;
    int i, animID;
    bool quit = false;

    tileset = AbelA_getTexture(tilesetID);
    entity = AbelE_newEntity(tileset, AbelV_i2fVec(AbelC_gridToPos(AbelV_newiVec2(0, 0))));
    animID = AbelS_addAnimation(entity->sprite);
    AbelS_addFrame(entity->sprite, animID, 16, 1000); /* tile id 16 for 1 second */
    AbelS_addFrame(entity->sprite, animID, 17, 100);  /* tile id 17 for .1 seconds */
    AbelS_playAnimation(entity->sprite, animID);      /* play animation :D */

    // AbelE_setVelocity(entity, AbelV_newfVec2(16, 16));

    AbelM_setCell(AbelV_newiVec2(3, 4), tileset, 4, true);
    AbelM_setCell(AbelV_newiVec2(3, 5), tileset, 4, true);
    AbelM_setCell(AbelV_newiVec2(3, 6), tileset, 4, true);
    AbelM_setCell(AbelV_newiVec2(4, 6), tileset, 4, true);
    AbelM_setCell(AbelV_newiVec2(5, 6), tileset, 4, true);

    /* main engine loop */
    while (!quit) {
        /* handle SDL events */
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
        }

        /* run scheduled tasks */
        AbelT_pollTasks();

        AbelR_getCamera()->pos.x = entity->sprite->pos.x + (AbelR_tileSize.x / 2);
        AbelR_getCamera()->pos.y = entity->sprite->pos.y + (AbelR_tileSize.y / 2);

        /* clear layers */
        SDL_RenderClear(AbelR_getRenderer());

        /* render chunks */
        AbelM_renderChunks(LAYER_BG);
        AbelM_renderEntities();

        /* render to window */
        SDL_RenderPresent(AbelR_getRenderer());
    }

    AbelE_freeEntity(entity);
}