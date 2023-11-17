#include "player.h"

#include "assets.h"
#include "core/mem.h"
#include "input.h"
#include "world.h"

static void freePlayer(tAbelM_refCount *ptr)
{
    tAbelP_player *player = (tAbelP_player *)ptr;

    AbelE_cleanupEntity(&player->entity);
    AbelI_onKeyDownDisconnect(player->onKeyDown);
    AbelI_onKeyUpDisconnect(player->onKeyUp);
    AbelW_onStepDisconnect(player->onStep);
    AbelM_free(player);
}

static void onKeyDown(const void *uData, const void *eventData)
{
    tAbelP_player *player = (tAbelP_player *)uData;
    const SDL_Event *evnt = (const SDL_Event *)eventData;

    switch (evnt->key.keysym.sym) {
    case SDLK_UP:
        player->selectedYDir = -1;
        break;
    case SDLK_DOWN:
        player->selectedYDir = 1;
        break;
    case SDLK_LEFT:
        player->selectedXDir = -1;
        break;
    case SDLK_RIGHT:
        player->selectedXDir = 1;
        break;
    default:
        break;
    }
}

static void onKeyUp(const void *uData, const void *eventData)
{
    tAbelP_player *player = (tAbelP_player *)uData;
    const SDL_Event *evnt = (const SDL_Event *)eventData;

    switch (evnt->key.keysym.sym) {
    case SDLK_UP:
        if (player->selectedYDir == -1) {
            player->selectedYDir = 0;
        }
        break;
    case SDLK_DOWN:
        if (player->selectedYDir == 1) {
            player->selectedYDir = 0;
        }
        break;
    case SDLK_LEFT:
        if (player->selectedXDir == -1) {
            player->selectedXDir = 0;
        }
        break;
    case SDLK_RIGHT:
        if (player->selectedXDir == 1) {
            player->selectedXDir = 0;
        }
        break;
    default:
        break;
    }
}

static void onStep(const void *uData, const void *eventData)
{
    tAbelP_player *player = (tAbelP_player *)uData;

    /* normalize direction, and multiply by the walkspeed to get the velocity */
    tAbelV_fVec2 vel = tAbelV_normalizefVec2(AbelV_newfVec2(player->selectedXDir, player->selectedYDir));
    vel = AbelV_mulfVec2(vel, AbelV_newfVec2(player->walkSpeed, player->walkSpeed));
    AbelE_setVelocity(&player->entity, vel);

    /* set animation */
    if (player->selectedYDir == 1 && player->selectedXDir == 0) {
        AbelS_playAnimation(&player->entity.sprite, player->downAnim);
    } else if (player->selectedYDir == -1 && player->selectedXDir == 0) {
        AbelS_playAnimation(&player->entity.sprite, player->upAnim);
    } else if (player->selectedYDir == 1 && player->selectedXDir == 1) {
        AbelS_playAnimation(&player->entity.sprite, player->downRightAnim);
    } else if (player->selectedYDir == 1 && player->selectedXDir == -1) {
        AbelS_playAnimation(&player->entity.sprite, player->downLeftAnim);
    } else if (player->selectedYDir == -1 && player->selectedXDir == 1) {
        AbelS_playAnimation(&player->entity.sprite, player->upRightAnim);
    } else if (player->selectedYDir == -1 && player->selectedXDir == -1) {
        AbelS_playAnimation(&player->entity.sprite, player->upLeftAnim);
    } else if (player->selectedYDir == 0 && player->selectedXDir == 1) {
        AbelS_playAnimation(&player->entity.sprite, player->rightAnim);
    } else if (player->selectedYDir == 0 && player->selectedXDir == -1) {
        AbelS_playAnimation(&player->entity.sprite, player->leftAnim);
    } else {
        /* default to idle animation */
        AbelS_playAnimation(&player->entity.sprite, player->idleAnim);
    }
}

static int setupAnimation(tAbelP_player *plr, int startTileID)
{
    int animId = AbelS_addAnimation(&plr->entity.sprite);

    AbelS_addFrame(&plr->entity.sprite, animId, startTileID + 1, 250);
    AbelS_addFrame(&plr->entity.sprite, animId, startTileID, 250);
    AbelS_addFrame(&plr->entity.sprite, animId, startTileID + 2, 250);
    AbelS_addFrame(&plr->entity.sprite, animId, startTileID, 250);
    return animId;
}

tAbelP_player *AbelP_newPlayer(tAbelV_fVec2 pos)
{
    tAbelR_texture *tileSet = AbelA_getTexture("res/entityset.png", AbelV_newiVec2(16, 16));
    tAbelP_player *player = (tAbelP_player *)AbelM_malloc(sizeof(tAbelP_player));
    player->onKeyDown = AbelI_onKeyDownConnect(onKeyDown, player);
    player->onKeyUp = AbelI_onKeyUpConnect(onKeyUp, player);
    player->onStep = AbelW_onStepConnect(onStep, player);
    AbelE_initEntity(&player->entity, tileSet, pos, freePlayer);

    /* animations */
    player->downAnim = setupAnimation(player, 0);
    player->downLeftAnim = setupAnimation(player, 3);
    player->downRightAnim = setupAnimation(player, 6);
    player->leftAnim = setupAnimation(player, 9);
    player->rightAnim = setupAnimation(player, 12);
    player->upAnim = setupAnimation(player, 16);
    player->upRightAnim = setupAnimation(player, 19);
    player->upLeftAnim = setupAnimation(player, 22);
    player->idleAnim = AbelS_addAnimation(&player->entity.sprite);
    AbelS_addFrame(&player->entity.sprite, player->idleAnim, 0, 100);

    player->walkSpeed = 64;
    player->selectedYDir = 0;
    player->selectedXDir = 0;

    AbelS_playAnimation(&player->entity.sprite, player->idleAnim);
    return player;
}

void AbelP_releasePlayer(tAbelP_player *player)
{
    AbelM_releaseRef(&player->entity.refCount);
}