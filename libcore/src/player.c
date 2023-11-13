#include "player.h"

#include "core/mem.h"

static void freePlayer(tAbelM_RefCount *ptr)
{
    tAbelP_player *player = (tAbelP_player *)ptr;
    AbelE_cleanupEntity(&player->entity);
    AbelM_free(player);
}

tAbelP_player *AbelP_newPlayer(tAbelV_fVec2 pos)
{
    tAbelP_player *player = (tAbelP_player *)AbelM_malloc(sizeof(tAbelP_player));
    player->walkSpeed = 16;
    player->selectedYDir = 0;
    player->selectedXDir = 0;

    AbelE_initEntity(&player->entity, pos, freePlayer);
    return player;
}

void AbelP_releasePlayer(tAbelP_player *player)
{
    AbelM_releaseRef(&player->entity.refCount);
}