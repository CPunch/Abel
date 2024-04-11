#ifndef ABEL_SOUND_H
#define ABEL_SOUND_H

#include "abel.h"
#include "core/mem.h"

typedef struct _tAbelK_sound
{
    tAbelM_refCount refCount;
    Mix_Chunk *chunk;
} tAbelK_sound;

void AbelK_init(uint32_t);
void AbelK_quit(void);

tAbelK_sound *AbelK_newSound(Mix_Chunk *chunk);
void AbelK_releaseSound(tAbelK_sound *sound);
void AbelK_playSound(tAbelK_sound *sound, int loops);

#endif