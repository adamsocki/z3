//
// Created by socki on 3/2/2025.
//

#include "Engine.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "RenderManager.h"
//#include "MaterialManager.h"
//#include "TextureManager.h"
#include "GameManager.h"
#include "tools/MaterialFactory.h"
#include "tools/GameObjectFactory.h"
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

        InitGameObjectFactory(engine);

//        InitMaterialManager(engine);


        InitRenderManager(&engine->renderManager, &engine->windowManager);





        InitGameManager(engine);


    }

    void UpdateEngine(Engine* engine)
    {

        UpdateTimeManager(engine);
        UpdateInputManager(engine);


        ClearInputManager(engine);

    }
} // Zayn