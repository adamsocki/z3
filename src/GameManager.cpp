//
// Created by socki on 3/5/2025.
//

#include "GameManager.h"
#include "Engine.h"
#include "game/entities/PlayerEntity.h"
#include "MyImgui.h"

namespace Zayn {

    void MakeTextures(Engine* engine)
    {
        // Texture - DynamicArray
        TextureCreateInfo texture1;
        texture1.path = "viking_room.png";
        texture1.name = "hi";
        MakeTexture(engine, &texture1);

        TextureCreateInfo cementTexture_1;
        cementTexture_1.path = "cementTexture_1.png";
        cementTexture_1.name = "cementTexture_1";
        MakeTexture(engine, &cementTexture_1);
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

        MaterialCreateInfo cementMat_1;
        cementMat_1.type = Game::MATERIAL_PBR;
        cementMat_1.texture = &engine->textureFactory.textures[1];
        cementMat_1.name = "cementMat_1";
        // @TODO: We need to set the proper descriptor set here.
        MakeMaterial(engine, &cementMat_1);
    }

    void MakeMeshes(Engine* engine)
    {
        MeshCreateInfo mesh1;
        mesh1.path = "viking_room.obj";
        mesh1.name = "Viking_Room";
        MakeMesh(engine, &mesh1);

        MeshCreateInfo mesh2;
        mesh2.path = "grand_piano.obj";
        mesh2.name = "Grand_Piano";
        MakeMesh(engine, &mesh2);

        MeshCreateInfo mesh3;
        mesh3.path = "floor.obj";
        mesh3.name = "FloorCement";
        MakeMesh(engine, &mesh3);
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

        playerEntity->health = 100;
        playerEntity->material = &engine->materialFactory.materials[0];
        playerEntity->mesh = &engine->meshFactory.meshes[0];
        playerEntity->pushConstantData.model_1 = TRS((V3(1.0f, 0.0f, 0.0f)), AxisAngle(V3(0.0f, 0.0f, 0.20f), 0.0f), V3(1.0f, 1.0f, 1.0f));

        playerEntity->name = "test Player 1";
        playerEntity->handle = engine->HTEST;


    }





    void GameInputUpdate(Zayn::InputManager* inputManager,Zayn::RenderManager* renderManager)
    {


#ifdef DEBUG
        if (InputPressed(inputManager->keyboard, Zayn::Input_F1))
        {
            ToggleImGuiVisibility(renderManager);
        }
#endif
    }

    void InitGameManager(Engine* engine)
    {
       MakeTextures(engine);
       MakeMaterials(engine);
       MakeMeshes(engine);

       MakeEntities(engine);

        // TODO: Put somewhere Better
        // MakeLevel(1);

    }

    void UpdateEntities(Engine* engine)
    {


        // Game::UpdateTransformPushConstants();


        Game::UpdateTransformPushConstants(engine);

    }


    void UpdateGameManager(Engine* engine)
    {
        Zayn::RenderManager* renderManager = &engine->renderManager;
        Zayn::InputManager* inputManager   = &engine->inputManager;
        GameInputUpdate(inputManager, renderManager);

        UpdateEntities(engine);

    }


} // Zayn