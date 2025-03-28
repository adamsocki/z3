//
// Created by socki on 3/12/2025.
//

#ifndef Z3_ENTITY_H
#define Z3_ENTITY_H

//#include "../../EntityManager.h"
#include <typeindex>

#include "../GameObject.h"
#include "../ModelPushConstant.h"
#include "../../tools/EntityFactory.h"
#include "../components/Component.h"

namespace Game {

    struct Entity {
        Mesh* mesh;
        Material* material;
        std::string name;
        glm::mat4 transform;

        Zayn::EntityHandle handle;

        ModelPushConstant pushConstantData;

        Zayn::DynamicArray<Component> components;

    };



} // Game

#endif //Z3_ENTITY_H
