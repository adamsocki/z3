//
// Created by socki on 3/2/2025.
//

#ifndef Z2_ENGINE_H
#define Z2_ENGINE_H

#include "MemoryManager.h"
#include "WindowManager.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "RenderManager.h"

#include "tools/TextureFactory.h"
#include "tools/MaterialFactory.h"
#include "tools/GameObjectFactory.h"
#include "tools/MeshFactory.h"
#include "tools/EntityFactory.h"
#include "CameraManager.h"

#define MAX_FRAMES_IN_FLIGHT 2


namespace Zayn {


    struct Engine{
        MemoryArena frameMemory;
        MemoryArena permanentMemory;

        WindowManager windowManager;

        TimeManager timeManager;
        InputManager inputManager;
        RenderManager renderManager;


        TextureFactory textureFactory;
        MaterialFactory materialFactory;
        MeshFactory meshFactory;
        EntityFactory entityFactory;

        Game::CameraManager cameraManager;
        EntityHandle HTEST;
        //        GameObjectFactory gameObjectFactory;

    };


    void InitEngine(Engine* engine);
    void UpdateEngine(Engine* engine);
} // Zayn

#endif //Z2_ENGINE_H
