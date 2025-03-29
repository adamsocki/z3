//
// Created by socki on 3/26/25.
//

#include "ComponentFactory.h"

namespace Zayn {





    void InitComponentFactory(ComponentFactory *factory, MemoryArena* arena) {

        // InitComponentStorage(&factory->componentStorage);

        factory->componentStorage.transformComponents = MakeDynamicArray<Game::TransformComponent>(arena, 128);

    }
} // Zayn