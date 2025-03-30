//
// Created by socki on 3/2/2025.
//

#ifndef Z2_ENGINE_H
#define Z2_ENGINE_H

#define IMGUI 1


#include "MemoryManager.h"
#include "WindowManager.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "RenderManager.h"

#include "tools/TextureFactory.h"
#include "tools/MaterialFactory.h"
#include "tools/MeshFactory.h"
#include "tools/EntityFactory.h"
#include "game/CameraManager.h"
#include "game/editor/LevelEditor.h"
#include "tools/ComponentFactory.h"

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
        ComponentFactory componentFactory;

        Game::CameraManager cameraManager;
        EntityHandle HTEST;

        LevelEditor levelEditor;
        bool inLevelEditor = false;
        //        GameObjectFactory gameObjectFactory;

    };


    void InitEngine(Engine* engine);
    void UpdateEngine(Engine* engine);
} // Zayn

#endif //Z2_ENGINE_H
