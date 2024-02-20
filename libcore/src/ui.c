#include "ui.h"

typedef struct
{
    tAbelU_Widget root;
} tAbelU_State;

static void freeWidget(tAbelM_refCount *ref)
{
    tAbelU_Widget *widget = (tAbelU_Widget *)ref;

    tAbelU_Widget *current = widget->childrenHead;
    while (current != NULL) {
        tAbelU_Widget *next = current->next;
        current->parent = NULL;
        AbelM_releaseRef(&current->refCount);
        current = next;
    }

    switch (widget->type) {
    case WIDGET_FRAME:
        break;
    case WIDGET_LABEL:
        AbelR_releaseTexture(((tAbelU_Label *)widget)->texture);
        AbelM_free(((tAbelU_Label *)widget)->text);
        break;
    default:
        printf("freeWidget: unknown widget type %d\n", widget->type);
        break;
    }

    ABEL_ASSERT(widget->parent == NULL); /* widget shouldn't have a parent */
    AbelM_free(widget);
}

static void initWidget(tAbelU_Widget *widget, WIDGET_TYPE type, tAbelV_iVec2 position, tAbelV_iVec2 size, SDL_Color color)
{
    AbelM_initRef(&widget->refCount, freeWidget);
    widget->type = type;
    widget->parent = NULL;
    widget->next = NULL;
    widget->childrenHead = NULL;
    widget->position = position;
    widget->size = size;
    widget->color = color;
}

static void removeChild(tAbelU_Widget *widget, tAbelU_Widget *child)
{
    tAbelU_Widget *current;

    child->parent = NULL;
    if (widget->childrenHead == child) {
        widget->childrenHead = child->next;
        goto exit;
    }

    current = widget->childrenHead;
    while (current != NULL && current->next != child)
        current = current->next;

    if (current)
        current->next = child->next;

exit:
    child->next = NULL;
    AbelM_releaseRef(&child->refCount);
}

static void addChild(tAbelU_Widget *widget, tAbelU_Widget *child)
{
    AbelM_retainRef(&child->refCount);

    if (child->parent != NULL)
        removeChild(child->parent, child);

    child->parent = widget;
    if (widget->childrenHead == NULL) {
        widget->childrenHead = child;
        return;
    }

    tAbelU_Widget *current = widget->childrenHead;
    while (current->next != NULL)
        current = current->next;

    if (current)
        current->next = child;
}

static void renderWidget(tAbelU_Widget *widget, tAbelV_iVec2 offset)
{
    switch (widget->type) {
    case WIDGET_FRAME:
        SDL_RenderFillRect(AbelR_getRenderer(), &(SDL_Rect){widget->position.x + offset.x, widget->position.y + offset.y, widget->size.x, widget->size.y});
        break;
    case WIDGET_LABEL:
        AbelR_renderTexture(((tAbelU_Label *)widget)->texture, NULL,
                            &(SDL_Rect){widget->position.x + offset.x, widget->position.y + offset.y, widget->size.x, widget->size.y});
        break;
    default:
        printf("AbelU_renderWidget: unknown widget type %d\n", widget->type);
        break;
    }

    tAbelU_Widget *current = widget->childrenHead;
    while (current != NULL) {
        renderWidget(current, AbelV_addiVec2(offset, widget->position));
        current = current->next;
    }
}

/* ========================================[[ API ]]======================================== */

void AbelU_init(uint32_t initFlags)
{
}

void AbelU_quit(void)
{
}

tAbelU_Frame *AbelU_newFrame(tAbelV_iVec2 position, tAbelV_iVec2 size, SDL_Color color)
{
    tAbelU_Frame *frame = AbelM_malloc(sizeof(tAbelU_Frame));
    initWidget(&frame->widget, WIDGET_FRAME, position, size, color);

    return frame;
}

tAbelU_Label *AbelU_newLabel(tAbelV_iVec2 position, tAbelV_iVec2 size, SDL_Color color, char *text)
{
    tAbelU_Label *label = AbelM_malloc(sizeof(tAbelU_Label));
    initWidget(&label->widget, WIDGET_LABEL, position, size, color);
    label->text = NULL;
    label->texture = NULL;

    AbelU_setLabelText(label, text, 0);
    return label;
}

void AbelU_releaseWidget(tAbelU_Widget *widget)
{
    AbelM_releaseRef(&widget->refCount);
}

void AbelU_setLabelText(tAbelU_Label *label, char *text, int keepsrc)
{
    /* if the text is the same, don't do anything */
    if (label->text && memcmp(label->text, text, strlen(text)) == 0) {
        if (keepsrc)
            AbelM_free(text);
        return;
    }

    /* free the old text */
    if (label->text)
        AbelM_free(label->text);

    /* copy the new text */
    if (!keepsrc) {
        label->text = AbelM_malloc(strlen(text) + 1);
        memcpy(label->text, text, strlen(text) + 1);
    } else {
        label->text = text;
    }

    /* render new texture */
    if (label->texture)
        AbelR_releaseTexture(label->texture);
    label->texture = AbelR_createText(NULL, text);
    label->widget.size = label->texture->size;
}

void AbelU_setLabelTextf(tAbelU_Label *label, char *format, ...)
{
    va_list args;
    va_start(args, format);
    int size = vsnprintf(NULL, 0, format, args);
    va_end(args);

    char *text = AbelM_malloc(size + 1);
    va_start(args, format);
    vsnprintf(text, size + 1, format, args);
    va_end(args);

    AbelU_setLabelText(label, text, 1);
}

void AbelU_addChild(tAbelU_Widget *parent, tAbelU_Widget *child)
{
    addChild(parent, child);
}

void AbelU_removeChild(tAbelU_Widget *parent, tAbelU_Widget *child)
{
    removeChild(parent, child);
}

void AbelU_renderWidget(tAbelU_Widget *widget)
{
    renderWidget(widget, AbelV_newiVec2(0, 0));
}