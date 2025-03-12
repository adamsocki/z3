//
// Created by socki on 3/5/2025.
//

#include <iostream>
#include "GameObjectFactory.h"
#include "../Engine.h"

namespace Zayn {


    void MakeGameObject(Engine* engine, Game::GameObject* gameObject)
    {
        std::cout << "game object Make" << std::endl;
    }

    void InitGameObjectFactory(Engine* engine)
    {
        engine->gameObjectFactory.gameObjects = MakeDynamicArray<Game::GameObject>(&engine->permanentMemory, 100);
    }


} // Zayn