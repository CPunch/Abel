#include "world.h"
#include "entity.h"

#include "core/hashmap.h"

static struct hashmap *AbelW_entityMap;
static ENTITY_ID AbelW_nextEntityID;

/* ===================================[[ Helper Functions ]]==================================== */

typedef struct
{
    ENTITY_ID id;
    tAbelE_entity *entity;
} tAbelW_entityElem;

uint64_t entityHash(const void *item, uint64_t seed0, uint64_t seed1)
{
    tAbelW_entityElem *elem = (tAbelW_entityElem *)item;
    return elem->id;
}

int entityCompare(const void *a, const void *b, void *udata)
{
    tAbelW_entityElem *elem1 = (tAbelW_entityElem *)a;
    tAbelW_entityElem *elem2 = (tAbelW_entityElem *)b;
    return elem1->id != elem2->id;
}

ENTITY_ID getNextEntityID()
{
    return AbelW_nextEntityID++;
}

ENTITY_ID addEntity(tAbelE_entity *entity)
{
    ENTITY_ID nextID = getNextEntityID();

    /* add to entity map */
    hashmap_set(AbelW_entityMap, &(tAbelW_entityElem){.id = nextID, .entity = entity});

    return nextID;
}

void rmvEntity(ENTITY_ID id)
{
    hashmap_delete(AbelW_entityMap, &(tAbelW_entityElem){.id = id});
}

tAbelE_entity *getEntity(ENTITY_ID id)
{
    tAbelW_entityElem *elem = hashmap_get(AbelW_entityMap, &(tAbelW_entityElem){.id = id});

    return elem ? elem->entity : NULL;
}

/* =====================================[[ Initializers ]]====================================== */

void AbelW_init(void)
{
    AbelW_entityMap = hashmap_new(sizeof(tAbelW_entityElem), 8, 0, 0, entityHash, entityCompare, NULL, NULL);
}

void AbelW_quit(void)
{
    hashmap_free(AbelW_entityMap);
}

ENTITY_ID AbelW_addEntity(tAbelE_entity *entity)
{
    return addEntity(entity);
}

void AbelW_rmvEntity(ENTITY_ID id)
{
    return rmvEntity(id);
}

tAbelE_entity *AbelW_getEntity(ENTITY_ID id)
{
    return getEntity(id);
}