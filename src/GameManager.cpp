//
// Created by socki on 3/5/2025.
//

#include "GameManager.h"
#include "game/GameObject.h"
#include "tools/GameObjectFactory.h"

namespace Zayn {

    struct Engine;

    void MakeTextures()
    {

    }

    void MakeMaterials()
    {

    }

    void GameObjects(Engine* engine)
    {

        Game::Texture texture;
        Game::Material material;
        Game::GameObject gameObject;






        MakeGameObject(engine, &gameObject);

    }





    void InitGameManager(Engine* engine)
    {

       // Game::MakeTextures()

       MakeTextures();

       MakeMaterials();

       GameObjects(engine);




    }
} // Zayn