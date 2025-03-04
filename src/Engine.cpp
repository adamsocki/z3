//
// Created by socki on 3/2/2025.
//

#include "Engine.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "RenderManager.h"


namespace Zayn {


    void InitEngine(Engine* engine)
    {
        InitMemoryManager(engine);
        InitWindowManager(engine, V2(100,133), "spanglish");
        InitTimeManager(engine);
        InitInputManager(engine);

        InitRenderManager(&engine->renderManager, &engine->windowManager);
    }

    void UpdateEngine(Engine* engine)
    {

        UpdateTimeManager(engine);
        UpdateInputManager(engine);


        ClearInputManager(engine);

    }
} // Zayn