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
        MaterialCreateInfo material1;
        material1.type = Game::MATERIAL_PBR;
        material1.texture = &engine->textureFactory.textures[0];
        material1.name = "mat1";
        // @TODO: We need to set the proper descriptor set here.
        MakeMaterial(engine, &material1);
    }

    void MakeMeshes(Engine* engine)
    {

    }

    void GameObjects(Engine* engine)
    {
        // GameObject - DynamicArray

        GameObjectCreateInfo gameObject1;
        gameObject1.material = &engine->materialFactory.materials[0];
//        gameObject1.mesh

//        Game::Texture texture;
//        Game::Material material;
//        Game::GameObject gameObject;


//        MakeGameObject(engine, gameObjects, &gameObject);
    }





    void InitGameManager(Engine* engine)
    {
       MakeTextures(engine);
       MakeMaterials(engine);
       MakeMeshes(engine);
       GameObjects(engine);
    }
} // Zayn