#include "entity.h"

#include "core/mem.h"
#include "world.h"

/* BOILERPLATE !!!! */

tAbelE_entity *AbelE_newEntity()
{
    tAbelE_entity *entity = (tAbelE_entity *)AbelM_malloc(sizeof(tAbelE_entity));

    entity->id = AbelW_addEntity(entity);
    return entity;
}

void AbelE_freeEntity(tAbelE_entity *entity)
{
    AbelW_rmvEntity(entity->id);
    AbelM_free(entity);
}