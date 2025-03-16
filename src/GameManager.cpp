//
// Created by socki on 3/5/2025.
//

#include "GameManager.h"
#include "Engine.h"
#include "game/entities/PlayerEntity.h"

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
        MeshCreateInfo mesh1;
        mesh1.path = "viking_room.obj";
        mesh1.name = "V1";
        MakeMesh(engine, &mesh1);
    }

    void MakeEntities(Engine* engine)
    {
        //EntityCreateInfo info = {};





        EntityFactory* entityFactory = &engine->entityFactory;
        InitEntityBuffers(entityFactory);

        if (false)
        {
            //MakeEntityBuffer<EntityType>
        }
        AddEntity(entityFactory, &engine->HTEST, Game::EntityType_Player);
        Game::PlayerEntity* playerEntity = (Game::PlayerEntity*)GetEntity(entityFactory, engine->HTEST);
//        Game::PlayerEntity playerEntity = {};

        playerEntity->health = 100;
        playerEntity->material = &engine->materialFactory.materials[0];
        playerEntity->mesh = &engine->meshFactory.meshes[0];
//        playerEntity->transform = glm::mat4();
        playerEntity->pushConstantData.model_1 = TRS((V3(1.0f, 0.0f, -10.0f)), AxisAngle(V3(0.0f, 0.0f, 0.20f), 0.0f), V3(1.0f, 1.0f, 1.0f));

        playerEntity->name = "test Player 1";
        playerEntity->handle = engine->HTEST;


    }





    void InitGameManager(Engine* engine)
    {
       MakeTextures(engine);
       MakeMaterials(engine);
       MakeMeshes(engine);

       MakeEntities(engine);


    }


    void UpdateGameManager(Engine* engine)
    {


        UpdateCameraManager(&engine->cameraManager, &engine->inputManager, &engine->timeManager);


    }


} // Zayn