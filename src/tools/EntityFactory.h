//
// Created by socki on 3/12/2025.
//

#ifndef Z3_ENTITYFACTORY_H
#define Z3_ENTITYFACTORY_H


#include "../data_types.h"
//#include "../game/entities/Entity.h"
//#include "../game/entities/Entity.h"
#include "../game/entities/EntityTypes.h"
#include <functional>

namespace Zayn {
    struct Engine;
    struct LevelEditor;

    struct FreeList {
        int32 freeList[1000];
        int32 freelistCount;
    };

    struct EntityHandle {
        int32 generation;
        int32 indexInInfo;
        Game::EntityType type;

        bool operator<(const EntityHandle& other) const {
            if (indexInInfo != other.indexInInfo) return indexInInfo < other.indexInInfo;
            if (generation != other.generation) return generation < other.generation;
            return type < other.type;
        }

        // For std::unordered_map
        bool operator==(const EntityHandle& other) const {
            return indexInInfo == other.indexInInfo &&
                   generation == other.generation &&
                   type == other.type;
        }
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

    namespace LE {

        struct EntityCreator {
            Game::EntityType selectedEntityType = Game::EntityType::EntityType_Count;

        };

        void CreateEntity(Engine* engine, EntityFactory* entityFactory, EntityCreator entityCreator, LevelEditor* levelEditor);

    }




} // Zayn

namespace std {
    template <>
    struct hash<Zayn::EntityHandle> {
        size_t operator()(const Zayn::EntityHandle& handle) const noexcept {
            // Simple hash combining fields - adjust if needed for better distribution
            size_t h1 = std::hash<int32>{}(handle.indexInInfo);
            size_t h2 = std::hash<int32>{}(handle.generation);
            size_t h3 = std::hash<int>{}(static_cast<int>(handle.type)); // Hash the underlying int
            // Combine hashes (example using XOR and shifts)
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}



#endif //Z3_ENTITYFACTORY_H
