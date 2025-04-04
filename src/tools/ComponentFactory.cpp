//
// Created by socki on 3/26/25.
//

#include "ComponentFactory.h"

namespace Zayn {





    void InitComponentFactory(ComponentFactory *factory, MemoryArena* arena) {

        // InitComponentStorage(&factory->componentStorage);

        factory->componentStorage.transformComponents = MakeDynamicArray<Game::TransformComponent>(arena, 1280);
        factory->componentStorage.renderComponents = MakeDynamicArray<Game::RenderComponent>(arena, 1280);
        factory->componentStorage.collisionComponents = MakeDynamicArray<Game::CollisionComponent>(arena, 1280);
        factory->componentStorage.physicsComponents = MakeDynamicArray<Game::PhysicsComponent>(arena, 1280);
        factory->componentStorage.playerControllers = MakeDynamicArray<Game::PlayerController>(arena, 128);

    }
} // Zayn