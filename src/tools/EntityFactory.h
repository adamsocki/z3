//
// Created by socki on 3/12/2025.
//

#ifndef Z3_ENTITYFACTORY_H
#define Z3_ENTITYFACTORY_H


#include "../data_types.h"
//#include "../game/entities/Entity.h"
//#include "../game/entities/Entity.h"
#include "../game/entities/EntityTypes.h"

namespace Zayn {

    struct FreeList {
        int32 freeList[1000];
        int32 freelistCount;
    };

    struct EntityHandle {
        int32 generation;
        int32 indexInInfo;
        Game::EntityType type;
    };


    struct EntityInfo {
        int32 generation;
        int32 indexInBuffer;

        Game::EntityType type;
    };

    struct EntityTypeBuffer {
        int32 count;
        int32 capacity;
        int32 sizeInBytes;

        void* entities;
    };


    struct EntityFactory {
        EntityTypeBuffer buffers[Game::EntityType_Count];
        EntityInfo *entities;
        int32 entityCapacity;

        int32 freeListCount;
        int32 freeList[10000];

//        FreeList levelIDtest[10];

        int32 nextID;
    };
    void* GetEntity(EntityFactory* entityFactory, EntityHandle handle);
//    void DeleteEntity();
//    EntityHandle AddEntity();
    void AddEntity(EntityFactory* entityFactory, EntityHandle* handle, Game::EntityType type);

    void InitEntityBuffers(EntityFactory* entityFactory);
    void InitEntityFactory(EntityFactory* entityFactory);

    namespace LE{

        struct EntityCreator {
            Game::EntityType selectedEntityType;

        };
    }


} // Zayn



#endif //Z3_ENTITYFACTORY_H
