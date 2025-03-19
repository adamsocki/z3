//
// Created by socki on 3/2/2025.
//


#include "Engine.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "GameManager.h"
#include "tools/MaterialFactory.h"
#include "tools/MeshFactory.h"


namespace Zayn {

    void InitEngine(Engine* engine)
    {
        InitMemoryManager(engine);
        InitWindowManager(engine, V2(500,533), "spanglish");
        InitTimeManager(engine);
        InitInputManager(engine);

        InitTextureFactory(engine);
        InitMaterialFactory(engine);
        InitMeshFactory(engine);

        InitEntityFactory(&engine->entityFactory);

//        InitGameObjectFactory(engine);
//        InitMaterialManager(engine);

        InitCameraManager( &engine->cameraManager, engine->windowManager.glfwWindow, &engine->inputManager);


        InitRenderManager(&engine->renderManager, &engine->windowManager);

//        InitCameraManager(engine);

        InitGameManager(engine);
    }

    void UpdateEngine(Engine* engine)
    {
        // Start OF FRAME
        UpdateTimeManager(engine);
        UpdateInputManager(engine);
        UpdateCameraManager(&engine->windowManager, &engine->cameraManager, &engine->inputManager, &engine->timeManager);

        // START OF GAME Frame
        UpdateGameManager(engine);

        UpdateRenderManager(engine, engine->HTEST, &engine->renderManager, &engine->windowManager, &engine->cameraManager, &engine->inputManager);


        // End OF FRAME
        ClearInputManager(engine);

    }
} // Zayn