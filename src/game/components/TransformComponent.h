//
// Created by socki on 3/24/25.
//

#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include "Component.h"
#include "../../math/math.h"
// #include "../../tools/ComponentFactory.h"
// #include "../../Engine.h"

namespace Game {

struct TransformComponent : Component {
    Zayn::EntityHandle owner;
    mat4 transform = Identity4();
    vec3 position = V3(0, 0, 0);
    vec3 rotation = V3(0, 0, 0);
    vec3 scale = V3(1, 1, 1);
};

    // void UpdateTransformPushConstants(Zayn::ComponentStorage *storage, Zayn::EntityFactory* entityFactory);


    void UpdateTransformPushConstants(Zayn::Engine* engine);

} // Game

#endif //TRANSFORMCOMPONENT_H
