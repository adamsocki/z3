//
// Created by socki on 3/24/25.
//

#include "TransformComponent.h"

#include "../../Engine.h"
#include "../entities/Entity.h"


namespace Zayn {
    struct ComponentFactory;
}

namespace Game {

    void UpdateTransformPushConstants(Zayn::Engine *engine) {
        Zayn::ComponentFactory * componentfactory = &engine->componentFactory;
        Zayn::ComponentStorage* storage = &componentfactory->componentStorage;

        Zayn::EntityFactory* entityFactory = &engine->entityFactory;

        if (!storage || !entityFactory) return;

        for (u32 i = 0; i < storage->transformComponents.count; ++i) {
                TransformComponent* tc = &storage->transformComponents[i];

                // Get the corresponding Entity pointer using the owner handle
                // Assuming Game::Entity is the base struct GetEntity returns a pointer to
                Game::Entity* entity = static_cast<Game::Entity*>(GetEntity(entityFactory, tc->owner));

                if (entity) {
                    mat4 modelMatrix = TRS(tc->position, AxisAngle(tc->rotation, 1.0f), tc->scale);

                    entity->pushConstantData.model_1 = modelMatrix;

                }

            }

    }

} // Game