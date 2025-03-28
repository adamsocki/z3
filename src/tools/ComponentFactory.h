//
// Created by socki on 3/26/25.
//

#ifndef COMPONENTFACTORY_H
#define COMPONENTFACTORY_H
#include "../DynamicArray.h"

#include "../game/components/TransformComponent.h"

namespace Zayn {


    struct ComponentStorage {
        DynamicArray<Game::TransformComponent> transformComponents;
    };
struct ComponentFactory {

    ComponentStorage componentStorage;

};

    void AddComponent();
    void InitComponentFactory(ComponentFactory *factory, MemoryArena* arena);


} // Zayn

#endif //COMPONENTFACTORY_H
