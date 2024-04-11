#include "sound.h"

static void freeSound(tAbelM_refCount *refCount)
{
    tAbelR_sound *sound = (tAbelR_sound *)refCount;
    printf("freeing sound %p\n", sound->chunk);
    Mix_FreeChunk(sound->chunk);
    AbelM_free(sound);
}

tAbelR_sound *AbelR_newSound(Mix_Chunk *chunk)
{
    tAbelR_sound *sound = AbelM_malloc(sizeof(tAbelR_sound));
    AbelM_initRef(&sound->refCount, freeSound);

    sound->chunk = chunk;
    return sound;
}

void AbelR_releaseSound(tAbelR_sound *sound)
{
    AbelM_releaseRef(&sound->refCount);
}

void AbelR_playSound(tAbelR_sound *sound, int loops)
{
    Mix_PlayChannel(-1, sound->chunk, loops);
}