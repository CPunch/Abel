#include "input.h"

#include "game.h"
#include "render.h"

typedef struct _tAbelI_state
{
    /* passes SDL_Event *evnt as eventData */
    tAbelVM_eventConnection *onKeyDown;
    tAbelVM_eventConnection *onKeyUp;
} tAbelI_state;

static tAbelI_state state = {0};

void AbelI_init(void)
{
    state.onKeyDown = NULL;
    state.onKeyUp = NULL;
}

void AbelI_quit(void)
{
    AbelVM_clearEventList(&state.onKeyDown);
    AbelVM_clearEventList(&state.onKeyUp);
}

void AbelI_onKeyDownConnect(tEventCallback callback, const void *uData)
{
    AbelVM_connectEvent(&state.onKeyDown, callback, uData);
}

void AbelI_onKeyUpConnect(tEventCallback callback, const void *uData)
{
    AbelVM_connectEvent(&state.onKeyUp, callback, uData);
}

void AbelI_pollEvents(void)
{
    SDL_Event evnt;
    struct nk_context *ctx = AbelR_getNuklearCtx();

    /* handle SDL events */
    nk_input_begin(ctx);
    while (SDL_PollEvent(&evnt) != 0) {
        switch (evnt.type) {
        case SDL_QUIT:
            AbelG_quit();
            break;
        case SDL_WINDOWEVENT:
            switch (evnt.window.event) {
            case SDL_WINDOWEVENT_RESIZED: {
                int width = evnt.window.data1;
                int height = evnt.window.data2;
                AbelR_setCameraSize(AbelV_newiVec2(width, height));
                break;
            }
            }
            break;
        case SDL_KEYDOWN:
            AbelVM_fireEventList(state.onKeyDown, &evnt);
            break;
        case SDL_KEYUP:
            AbelVM_fireEventList(state.onKeyUp, &evnt);
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
}