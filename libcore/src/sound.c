#include "sound.h"

#include "core/init.h"
#include "core/serror.h"

typedef struct _tAbelK_state
{
    tAbelK_sound *playing[MIX_CHANNELS];
} tAbelK_state;

static tAbelK_state state = {0};

static void reset()
{
    state = (const tAbelK_state){0};
}

static inline void addPlaying(tAbelK_sound *sound, int chan)
{
    AbelM_retainRef(&sound->refCount);
    state.playing[chan] = sound;
}

static inline void removePlaying(int chan)
{
    AbelK_releaseSound(state.playing[chan]);
    state.playing[chan] = NULL;
}

static void freeSound(tAbelM_refCount *refCount)
{
    tAbelK_sound *sound = (tAbelK_sound *)refCount;

    printf("freeing sound %p\n", sound->chunk);
    Mix_FreeChunk(sound->chunk);
    AbelM_free(sound);
}

/* callback for when a sound finishes playing on a channel */
static void channelFinished(int chan)
{
    /* sanity check, then release sound */
    if (chan >= 0 && chan < MIX_CHANNELS && state.playing[chan] != NULL) {
        removePlaying(chan);
    }
}

void AbelK_init(uint32_t initFlags)
{
    if (!(initFlags & ABEL_INIT_NOAUDIO)) {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
            ABEL_ERROR("Failed to initialize SDL_MIXER: %s\n", Mix_GetError());

        Mix_ChannelFinished(channelFinished);
    }

    reset();
}

void AbelK_quit(void)
{
    for (int i = 0; i < MIX_CHANNELS; i++) {
        if (state.playing[i]) {
            removePlaying(i);
        }
    }

    Mix_CloseAudio();

    /* asan won't actually mark any globally stored pointers as
     memory leaks */
    reset();
}

tAbelK_sound *AbelK_newSound(Mix_Chunk *chunk)
{
    tAbelK_sound *sound = AbelM_malloc(sizeof(tAbelK_sound));
    AbelM_initRef(&sound->refCount, freeSound);

    sound->chunk = chunk;
    return sound;
}

void AbelK_releaseSound(tAbelK_sound *sound)
{
    AbelM_releaseRef(&sound->refCount);
}

void AbelK_playSound(tAbelK_sound *sound, int loops)
{
    int chan = Mix_PlayChannel(-1, sound->chunk, loops);

    if (chan >= 0) {
        addPlaying(sound, chan);
    }
}