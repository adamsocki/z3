//
// Created by socki on 3/5/2025.
//

#ifndef Z3_GAMEOBJECTFACTORY_H
#define Z3_GAMEOBJECTFACTORY_H

#include "../game/GameObject.h"
#include "../DynamicArray.h"
#include "../RenderManager.h"

namespace Zayn {

    struct GameObjectFactory{
        DynamicArray<Game::GameObject> gameObjects;
    };



    void MakeGameObject(Engine* engine, Game::GameObject* gameObject);
    void InitGameObjectFactory(Engine* engine);

} // Zayn

#endif //Z3_GAMEOBJECTFACTORY_H
