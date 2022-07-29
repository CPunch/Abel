#include "map.h"

#include "core/hashmap.h"
#include "core/tasks.h"
#include "entity.h"
#include "game.h"
#include "layer.h"

static struct hashmap *AbelM_entityMap;
static struct hashmap *AbelM_cellMap;
static ENTITY_ID AbelM_nextEntityID = 0;
static tAbelT_task *AbelM_stepTimer;

/* ===================================[[ Helper Functions ]]==================================== */

typedef struct
{
    ENTITY_ID id;
    tAbelE_entity *entity;
} tAbelM_entityElem;

typedef struct 
{
    tAbel_iVec2 pos;
    tAbelM_cell cell;
} tAbelM_cellElem;

/* cell */
static uint64_t cellHash(const void *item, uint64_t seed0, uint64_t seed1)
{
    tAbelM_cellElem *elem = (tAbelM_cellElem *)item;
    int hash = 23;
    hash = (hash * 37) + elem->pos.x;
    hash = (hash * 37) + elem->pos.y;
    return hash;
}

static int cellCompare(const void *a, const void *b, void *udata) {
    tAbelM_cellElem *elem1 = (tAbelM_cellElem *)a;
    tAbelM_cellElem *elem2 = (tAbelM_cellElem *)b;
    return elem1->pos.x != elem2->pos.x || elem1->pos.y != elem2->pos.y;
}

/* entity */
static uint64_t entityHash(const void *item, uint64_t seed0, uint64_t seed1)
{
    tAbelM_entityElem *elem = (tAbelM_entityElem *)item;
    return elem->id;
}

static int entityCompare(const void *a, const void *b, void *udata)
{
    tAbelM_entityElem *elem1 = (tAbelM_entityElem *)a;
    tAbelM_entityElem *elem2 = (tAbelM_entityElem *)b;
    return elem1->id != elem2->id;
}

static ENTITY_ID getNextEntityID()
{
    return AbelM_nextEntityID++;
}

static ENTITY_ID addEntity(tAbelE_entity *entity)
{
    ENTITY_ID nextID = getNextEntityID();

    /* add to entity map */
    hashmap_set(AbelM_entityMap, &(tAbelM_entityElem){.id = nextID, .entity = entity});

    return nextID;
}

static void rmvEntity(ENTITY_ID id)
{
    hashmap_delete(AbelM_entityMap, &(tAbelM_entityElem){.id = id});
}

static tAbelE_entity *getEntity(ENTITY_ID id)
{
    tAbelM_entityElem *elem = hashmap_get(AbelM_entityMap, &(tAbelM_entityElem){.id = id});

    return elem ? elem->entity : NULL;
}

static uint32_t worldStepTask(uint32_t delta, void *uData)
{
    size_t i = 0;
    void *item;
    tAbelM_entityElem *elem;

    while (hashmap_iter(AbelM_entityMap, &i, &item)) {
        elem = (tAbelM_entityElem *)item;
        AbelE_stepEntity(elem->entity, delta);
    }

    return WORLD_STEP_INTERVAL;
}

/* =====================================[[ Initializers ]]====================================== */

void AbelM_init(void)
{
    AbelM_entityMap =
        hashmap_new(sizeof(tAbelM_entityElem), 8, 0, 0, entityHash, entityCompare, NULL, NULL);
    AbelM_cellMap =
        hashmap_new(sizeof(tAbelM_cellElem), 8, 0, 0, cellHash, cellCompare, NULL, NULL);
    AbelM_stepTimer = AbelT_newTask(WORLD_STEP_INTERVAL, worldStepTask, NULL);
}

void AbelM_quit(void)
{
    hashmap_free(AbelM_entityMap);
    hashmap_free(AbelM_cellMap);
    AbelT_freeTask(AbelM_stepTimer);
}

/* ==========================================[[ Map ]]========================================== */

ENTITY_ID AbelM_addEntity(tAbelE_entity *entity)
{
    return addEntity(entity);
}

void AbelM_rmvEntity(ENTITY_ID id)
{
    return rmvEntity(id);
}

tAbelE_entity *AbelM_getEntity(ENTITY_ID id)
{
    return getEntity(id);
}

/* check for collision against other entities */
tAbelE_entity *AbelM_checkEntityCollide(tAbelE_entity *entity)
{
    size_t i = 0;
    void *item;
    tAbelE_entity *other;
    tAbel_fVec2 pos = AbelE_getPosition(entity), otherPos;
    tAbel_iVec2 collider = AbelE_getCollider(entity), otherCollider;

    /* walk through each entity, check for collision */
    while (hashmap_iter(AbelM_entityMap, &i, &item)) {
        other = ((tAbelM_entityElem *)item)->entity;
        otherPos = AbelE_getPosition(other);
        otherCollider = AbelE_getCollider(other);

        /* check collide */
        if (pos.x < otherPos.x + otherCollider.x &&
            pos.x + collider.x > otherPos.x &&
            pos.y < otherPos.y + otherCollider.y &&
            pos.y + collider.y > otherPos.y)
            return other;
    }

    /* no collisions found! */
    return NULL;
}

/* =========================================[[ Cells ]]========================================= */

void AbelM_setCell(tAbel_iVec2 pos, TILE_ID id, bool isSolid)
{
    hashmap_set(AbelM_cellMap, &(tAbelM_cellElem){
        .pos = pos,
        .cell = (tAbelM_cell){
            .id = id,
            .isSolid = isSolid
        }
    });

    /* update layer */
    AbelL_drawTile(AbelG_getLayer(LAYER_MAP), AbelL_gridToPos(pos), id, FRAME_BG);
}

tAbelM_cell AbelM_getCell(tAbel_iVec2 pos)
{
    tAbelM_cellElem *elem = hashmap_get(AbelM_cellMap, &(tAbelM_cellElem){.pos = pos});

    /* if cell doesn't exist, return default cell */
    return (elem == NULL) ? (tAbelM_cell){.id = 0, .isSolid = true} : elem->cell;
}