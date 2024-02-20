#ifndef ABEL_UI_H
#define ABEL_UI_H

#include "abel.h"
#include "core/event.h"
#include "core/mem.h"
#include "core/vec2.h"
#include "render.h"

/*
    tAbelU_Widget is the base struct for all widgets.
*/

typedef enum
{
    WIDGET_FRAME,
    WIDGET_LABEL
} WIDGET_TYPE;

typedef struct _tAbelU_Widget
{
    tAbelM_refCount refCount;
    WIDGET_TYPE type;
    tAbelV_iVec2 position;
    tAbelV_iVec2 size;
    SDL_Color color;
    struct _tAbelU_Widget *parent;
    struct _tAbelU_Widget *next;
    struct _tAbelU_Widget *childrenHead;
} tAbelU_Widget;

typedef struct _tAbelU_Frame
{
    tAbelU_Widget widget;
} tAbelU_Frame;

typedef struct _tAbelU_Label
{
    tAbelU_Widget widget;
    char *text;
    tAbelR_texture *texture;
} tAbelU_Label;

/* initializers */
void AbelU_init(uint32_t initFlags);
void AbelU_quit(void);

tAbelU_Frame *AbelU_newFrame(tAbelV_iVec2 position, tAbelV_iVec2 size, SDL_Color color);
tAbelU_Label *AbelU_newLabel(tAbelV_iVec2 position, tAbelV_iVec2 size, SDL_Color color, char *text);

void AbelU_releaseWidget(tAbelU_Widget *widget);

/* non-zero keepsrc won't malloc a new buffer for the text, and instead
 take ownership of the passed text pointer */
void AbelU_setLabelText(tAbelU_Label *label, char *text, int keepsrc);
void AbelU_setLabelTextf(tAbelU_Label *label, char *format, ...);

void AbelU_addChild(tAbelU_Widget *parent, tAbelU_Widget *child);
void AbelU_removeChild(tAbelU_Widget *parent, tAbelU_Widget *child);

void AbelU_renderWidget(tAbelU_Widget *widget);

#endif