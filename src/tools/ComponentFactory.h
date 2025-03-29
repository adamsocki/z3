//
// Created by socki on 3/26/25.
//

#ifndef COMPONENTFACTORY_H
#define COMPONENTFACTORY_H


#include "../DynamicArray.h"

#include "../game/components/TransformComponent.h"

// namespace Game {
//     struct Entity;
// }



namespace Zayn {

    enum ComponentType {
        TransformComponent_Type,


        ComponentType_Count
    };

    inline const char* GetComponentTypeName(ComponentType type) {
        switch (type) {
            case ComponentType::TransformComponent_Type:  return "Transform";
            // case ComponentType::RENDER:     return "Render";
            // --- Add cases for your other component types ---
            // case ComponentType::COLLISION: return "Collision";
            // case ComponentType::SCRIPT:    return "Script";
            // --- End of custom cases ---

            case ComponentType::ComponentType_Count:      // Fallthrough intentional for unknown types
                default:                        return "Unknown";
        }
    }



    struct ComponentStorage {
        DynamicArray<Game::TransformComponent> transformComponents;
    };

    struct ComponentFactory {

        ComponentStorage componentStorage;

    };


    inline bool AreHandlesEqual(EntityHandle h1, EntityHandle h2) {
        return h1.indexInInfo == h2.indexInInfo &&
               h1.generation == h2.generation &&
               h1.type == h2.type;
    }



    template<typename T>
    inline T* AddComponent(DynamicArray<T>* componentArray, EntityHandle entityHandle) {
        if (!componentArray) {
            printf("Error: Component array pointer is null.\n");
            return nullptr;
        }

        T newComp = {};
        newComp.owner = entityHandle;
        uint32 index = PushBack(componentArray, newComp);
        return &(*componentArray)[index];
    }


    template<typename T>
        inline T* FindComponentInArray(DynamicArray<T>* componentArray, EntityHandle entityHandle) {
        if (!componentArray) {
            return nullptr;
        }
        for (uint32 i = 0; i < componentArray->count; ++i) {
            // Compare the owner handle of the component in the array with the target handle
            if (AreHandlesEqual((*componentArray)[i].owner, entityHandle)) {
                // Found it, return pointer to the data in the array
                return &(*componentArray)[i];
            }
        }
        return nullptr; // Not found
    }




    void InitComponentFactory(ComponentFactory *factory, MemoryArena* arena);



} // Zayn

#endif //COMPONENTFACTORY_H
