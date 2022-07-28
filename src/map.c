#include "map.h"

#include "core/hashmap.h"
#include "core/tasks.h"
#include "entity.h"

static struct hashmap *AbelW_entityMap;
static ENTITY_ID AbelW_nextEntityID;
static tAbelT_task *AbelW_stepTimer;

/* ===================================[[ Helper Functions ]]==================================== */

typedef struct
{
    ENTITY_ID id;
    tAbelE_entity *entity;
} tAbelW_entityElem;

static uint64_t entityHash(const void *item, uint64_t seed0, uint64_t seed1)
{
    tAbelW_entityElem *elem = (tAbelW_entityElem *)item;
    return elem->id;
}

static int entityCompare(const void *a, const void *b, void *udata)
{
    tAbelW_entityElem *elem1 = (tAbelW_entityElem *)a;
    tAbelW_entityElem *elem2 = (tAbelW_entityElem *)b;
    return elem1->id != elem2->id;
}

static ENTITY_ID getNextEntityID()
{
    return AbelW_nextEntityID++;
}

static ENTITY_ID addEntity(tAbelE_entity *entity)
{
    ENTITY_ID nextID = getNextEntityID();

    /* add to entity map */
    hashmap_set(AbelW_entityMap, &(tAbelW_entityElem){.id = nextID, .entity = entity});

    return nextID;
}

static void rmvEntity(ENTITY_ID id)
{
    hashmap_delete(AbelW_entityMap, &(tAbelW_entityElem){.id = id});
}

static tAbelE_entity *getEntity(ENTITY_ID id)
{
    tAbelW_entityElem *elem = hashmap_get(AbelW_entityMap, &(tAbelW_entityElem){.id = id});

    return elem ? elem->entity : NULL;
}

static uint32_t worldStepTask(uint32_t delta, void *uData)
{
    size_t i = 0;
    void *item;
    tAbelW_entityElem *elem;

    while (hashmap_iter(AbelW_entityMap, &i, &item)) {
        elem = (tAbelW_entityElem *)item;
        AbelE_stepEntity(elem->entity, delta);
    }

    return WORLD_STEP_INTERVAL;
}

/* =====================================[[ Initializers ]]====================================== */

void AbelM_init(void)
{
    AbelW_entityMap =
        hashmap_new(sizeof(tAbelW_entityElem), 8, 0, 0, entityHash, entityCompare, NULL, NULL);
    AbelW_stepTimer = AbelT_newTask(WORLD_STEP_INTERVAL, worldStepTask, NULL);
}

void AbelM_quit(void)
{
    hashmap_free(AbelW_entityMap);
    AbelT_freeTask(AbelW_stepTimer);
}

/* =========================================[[ World ]]========================================= */

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
    while (hashmap_iter(AbelW_entityMap, &i, &item)) {
        other = ((tAbelW_entityElem *)item)->entity;
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