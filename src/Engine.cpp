//
// Created by socki on 3/2/2025.
//

#include "Engine.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "GameManager.h"


namespace Zayn {


    void InitEngine(Engine* engine)
    {
        InitMemoryManager(engine);
        InitWindowManager(engine, V2(100,133), "spanglish");
        InitTimeManager(engine);
        InitInputManager(engine);

        InitTextureManager(engine);




        InitMaterialManager(engine);

        InitGameManager(engine);

        InitRenderManager(&engine->renderManager, &engine->windowManager);


    }

    void UpdateEngine(Engine* engine)
    {

        UpdateTimeManager(engine);
        UpdateInputManager(engine);


        ClearInputManager(engine);

    }
} // Zayn