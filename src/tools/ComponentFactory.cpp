//
// Created by socki on 3/26/25.
//

#include "ComponentFactory.h"

namespace Zayn {


    template<typename T>
    inline T* AddComponent(DynamicArray<T>* componentArray, EntityHandle entityHandle) {
        if (!componentArray) {
            return nullptr;
        }

        T newComp = {}; // Default initialize the component struct
        newComp.owner = entityHandle; // Set the owner handle

        // PushBack returns the index of the added element
        uint32 index = PushBack(componentArray, newComp);

        return &(*componentArray)[index];
    }

    void InitComponentFactory(ComponentFactory *factory, MemoryArena* arena) {

        // InitComponentStorage(&factory->componentStorage);

        factory->componentStorage.transformComponents = MakeDynamicArray<Game::TransformComponent>(arena, 128);

    }
} // Zayn