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

namespace Zayn {


    struct Engine{
        MemoryArena frameMemory;
        MemoryArena permanentMemory;

        WindowManager windowManager;

        TimeManager timeManager;
        InputManager inputManager;
        RenderManager renderManager;
    };


    void InitEngine(Engine* engine);
    void UpdateEngine(Engine* engine);
} // Zayn

#endif //Z2_ENGINE_H
