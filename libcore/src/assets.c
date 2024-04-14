#include "assets.h"

#include "core/mem.h"
#include "core/serror.h"
#include "core/vec2.h"
#include "render.h"
#include "sound.h"

#include <zip.h>

typedef struct _tAbelA_state
{
    zip_t *archive;
} tAbelA_state;

static tAbelA_state state = {0};

static void closeArchive(void)
{
    if (state.archive) {
        zip_close(state.archive);
    }
}

/*
    build a test archive with:
    $ zip -0 data.zip -r res

    which will zip all of res into data.zip, with compression disabled (since we meed to support seeking)
*/
static bool openArchive(const char *path)
{
    int err;
    if (((state.archive = zip_open(path, ZIP_RDONLY, &err)) == NULL)) {
        zip_error_t error;
        zip_error_init_with_code(&error, err);
        ABEL_WARN("cannot open archive '%s': %s\n", path, zip_error_strerror(&error));
        zip_error_fini(&error);
        return false;
    }

    printf("using data archive %s!\n", path);
    return true;
}

/* ===================================[[ SDL_RWops wrapper ]]=================================== */

static Sint64 _size(SDL_RWops *ctx)
{
    zip_file_t *file = (zip_file_t *)ctx->hidden.unknown.data1;
    int cursor, size;

    cursor = zip_ftell(file);

    /* grab end */
    zip_fseek(file, 0, SEEK_END);
    size = zip_ftell(file);

    /* restore cursor */
    zip_fseek(file, cursor, SEEK_SET);
    return size;
}

static Sint64 _seek(SDL_RWops *ctx, Sint64 offset, int whence)
{
    zip_file_t *file = (zip_file_t *)ctx->hidden.unknown.data1;
    return (Sint64)zip_fseek(file, offset, whence);
}

static size_t _read(SDL_RWops *ctx, void *ptr, size_t size, size_t maxnum)
{
    zip_file_t *file = (zip_file_t *)ctx->hidden.unknown.data1;

    /* try to read maxnum objects sizeof size*/
    for (int i = 0; i < maxnum; i++) {
        int read;
        uint8_t *out = (uint8_t *)ptr + (i * size);
        if ((read = zip_fread(file, (void *)out, size)) != size) {
            return i;
        }
    }

    return maxnum;
}

static size_t _write(struct SDL_RWops *ctx, const void *ptr, size_t size, size_t num)
{
    ABEL_ERROR("UNIMPL: ATTEMPT TO WRITE TO FILE!")
}

static int _close(struct SDL_RWops *ctx)
{
    zip_file_t *file = (zip_file_t *)ctx->hidden.unknown.data1;
    return zip_fclose(file);
}

static SDL_RWops *fopenSDLArchive(const char *path)
{
    zip_file_t *file;

    if ((file = zip_fopen(state.archive, path, ZIP_FL_UNCHANGED)) == NULL) {
        ABEL_WARN("cannot open data '%s': %s\n", path, zip_error_strerror(zip_get_error(state.archive)));
        return NULL;
    }

    SDL_RWops *ctx = SDL_AllocRW();
    ctx->size = _size;
    ctx->seek = _seek;
    ctx->read = _read;
    ctx->write = _write;
    ctx->close = _close;

    ctx->type = SDL_RWOPS_UNKNOWN;
    ctx->hidden.unknown.data1 = (void *)file;
    return ctx;
}

/* wrapper to get a SDL_RWops from an opened archive, or default to filesystem */
static SDL_RWops *fopenSDLData(const char *path)
{
    if (state.archive) {
        return fopenSDLArchive(path);
    } else {
        return SDL_RWFromFile(path, "r");
    }
}

/* =======================================[[ Assets API ]]====================================== */

void AbelA_init(uint32_t initFlags)
{
    state = (tAbelA_state){0};
}

void AbelA_quit(void)
{
    closeArchive();
}

bool AbelA_openArchive(const char *path)
{
    closeArchive();
    return openArchive(path);
}

tAbelR_texture *AbelA_getTexture(const char *path, tAbelV_iVec2 tileSize)
{
    SDL_Texture *rawText;

    /* load raw texture */
    rawText = IMG_LoadTexture_RW(AbelR_getRenderer(), fopenSDLData(path), 1);
    if (rawText == NULL)
        ABEL_ERROR("Failed to load texture from '%s': %s\n", path, SDL_GetError());

    return AbelR_newTexture(rawText, tileSize);
}

tAbelK_sound *AbelA_getSound(const char *path)
{
    Mix_Chunk *chunk;

    /* load raw audio chunk */
    chunk = Mix_LoadWAV_RW(fopenSDLData(path), 1);
    if (chunk == NULL)
        ABEL_ERROR("Failed to load sound from AAAAAAAAAAAAA '%s': %s\n", path, Mix_GetError());

    return AbelK_newSound(chunk);
}
