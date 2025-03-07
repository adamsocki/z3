//
// Created by socki on 3/5/2025.
//

#include "GameManager.h"
#include "game/GameObject.h"
#include "Engine.h"
//#include "tools/TextureFactory.h"

namespace Zayn {

    void MakeTextures(Engine* engine)
    {
        // Texture - DynamicArray
        TextureCreateInfo texture1;
        texture1.path = "viking_room.png";
        texture1.name = "hi";
        MakeTexture(engine, &texture1);
    }

    void MakeMaterials(Engine* engine)
    {
        // Material - DynamicArray
    }

    void GameObjects(Engine* engine)
    {
        // GameObject - DynamicArray




//        Game::Texture texture;
//        Game::Material material;
//        Game::GameObject gameObject;


//        MakeGameObject(engine, gameObjects, &gameObject);
    }





    void InitGameManager(Engine* engine)
    {
       MakeTextures(engine);
       MakeMaterials(engine);
       GameObjects(engine);
    }
} // Zayn