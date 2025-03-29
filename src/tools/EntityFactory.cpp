//
// Created by socki on 3/12/2025.
//

#include <cstring>
#include <cstdlib>
#include "EntityFactory.h"
#include "../Engine.h"
#include "../game/entities/Entity.h"

namespace Game {
    struct Entity;
}

namespace Zayn {






    void* GetEntity(EntityFactory* entityFactory, EntityHandle handle) {
        if (handle.indexInInfo >= entityFactory->entityCapacity) {
            return NULL;
        }

        EntityInfo* info = &entityFactory->entities[handle.indexInInfo];

        if (info->generation != handle.generation) {
            return NULL;
        }
        if (info->type != handle.type) {
            return NULL;
        }

        EntityTypeBuffer* buffer = &entityFactory->buffers[info->type];

        return ((u8*)buffer->entities + (buffer->sizeInBytes * info->indexInBuffer));
    }

    void DeleteEntity(EntityFactory* em, EntityHandle handle) {
//
//
//        freeList[freeListCount] = handle.indexInInfo;
//        freeListCount++;
//
//        EntityTypeBuffer* buffer = &em->buffers[handle.type];
//        EntityInfo* info = &em->entities[handle.indexInInfo];
//        EntityHandle handleOfSwappedEntity;
//
//        buffer->count--;
    }

    void AddEntity(EntityFactory* entityFactory, EntityHandle* handle, Game::EntityType type) {
        int32 nextFreeIdInIndex = entityFactory->nextID;

        if (entityFactory->freeListCount > 0) {
            nextFreeIdInIndex = entityFactory->freeList[entityFactory->freeListCount - 1];
            entityFactory->freeListCount--;
        }
        else {
            entityFactory->nextID++;
        }

        EntityInfo* info = &entityFactory->entities[nextFreeIdInIndex];
        info->type = type;
        info->generation++;

        EntityTypeBuffer* buffer = &entityFactory->buffers[type];
        info->indexInBuffer = buffer->count;
        buffer->count++;

//        EntityHandle handle = {};
        handle->generation = info->generation;
        handle->indexInInfo = nextFreeIdInIndex;
        handle->type = type;

//        return handle;
    }


    void InitEntityFactory(EntityFactory* entityFactory) {
        // Entity Manager

        entityFactory->entityCapacity = 100000;
        entityFactory->entities = (EntityInfo*)malloc(sizeof(EntityInfo) * entityFactory->entityCapacity);
        memset(entityFactory->entities, 0, sizeof(EntityInfo) * entityFactory->entityCapacity);
        entityFactory->nextID = 0;

       // InitEntityBuffers();
    }

    void LE::CreateEntity(Engine* engine, EntityFactory* entityFactory, EntityCreator entityCreator, LevelEditor *levelEditor) {
        EntityHandle newEntityHandle = {};
        AddEntity(entityFactory, &newEntityHandle, entityCreator.selectedEntityType);

        Game::Entity* entity = static_cast<Game::Entity*>(GetEntity(entityFactory, newEntityHandle));


        entity->name = "default";

        entity->material = &engine->materialFactory.materials[0];
        entity->mesh = &engine->meshFactory.meshes[0];
        // entity->type =
        // entity->pushConstantData.model_1 = TRS((V3(1.0f, 0.0f, 0.0f)), AxisAngle(V3(0.0f, 0.0f, 0.20f), 0.0f), V3(1.0f, 1.0f, 1.0f));




        PushBack(&levelEditor->currentLevelData.levelEntityHandles, newEntityHandle);


        // levelEditor->currentLevelData
        // levelEditor->

    }

    void InitEntityBuffers(EntityFactory* entityFactory) {
        for (int i = 0; i < Game::EntityType_Count; i++)
        {
            Game::EntityTypeInfoForBuffer info = Game::entityTypeInfoForBuffer[i];
            EntityTypeBuffer* buffer = &entityFactory->buffers[i];
            buffer->capacity = info.defaultCapacity;
            buffer->sizeInBytes = info.structSize;
            buffer->count = 0;
            buffer->entities = malloc(buffer->capacity * buffer->sizeInBytes);
            memset(buffer->entities, 0, buffer->capacity * buffer->sizeInBytes);
        }
    }

} // Zayn