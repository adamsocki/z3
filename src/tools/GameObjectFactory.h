//
// Created by socki on 3/5/2025.
//

#ifndef Z3_GAMEOBJECTFACTORY_H
#define Z3_GAMEOBJECTFACTORY_H

#include "../game/GameObject.h"
#include "../DynamicArray.h"
#include "../RenderManager.h"
#include "../game/ModelPushConstant.h"

namespace Zayn {

    struct GameObjectCreateInfo
    {
        Game::Material* material;
        Game::Mesh* mesh;
        glm::mat4 transform;
        std::string name;
        Game::ModelPushConstant pushConstantData;

//        Entity
    };


    struct GameObjectFactory{
        DynamicArray<Game::GameObject> gameObjects;
    };



    void MakeGameObject(Engine* engine, Game::GameObject* gameObject, bool isEntityManaged);
    void InitGameObjectFactory(Engine* engine);

} // Zayn

#endif //Z3_GAMEOBJECTFACTORY_H
