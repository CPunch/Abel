#ifndef ABEL_SOUND_H
#define ABEL_SOUND_H

#include "abel.h"
#include "core/mem.h"

typedef struct _tAbelR_sound
{
    tAbelM_refCount refCount;
    Mix_Chunk *chunk;
} tAbelR_sound;

tAbelR_sound *AbelR_loadSound(const char *path);
void AbelR_releaseSound(tAbelR_sound *sound);

void AbelR_playSound(tAbelR_sound *sound, int loops);

#endif