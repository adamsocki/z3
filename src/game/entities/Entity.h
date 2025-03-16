//
// Created by socki on 3/12/2025.
//

#ifndef Z3_ENTITY_H
#define Z3_ENTITY_H

//#include "../../EntityManager.h"
#include "../GameObject.h"
#include "../ModelPushConstant.h"
#include "../../tools/EntityFactory.h"
//#include "../../tools/EntityFactory.h"
//#include "../../tools/EntityFactory.h"

namespace Game {

    struct Entity {
        Mesh* mesh;
        Material* material;
        std::string name;
        glm::mat4 transform;

        Zayn::EntityHandle handle;

        ModelPushConstant pushConstantData;

    };

} // Game

#endif //Z3_ENTITY_H
