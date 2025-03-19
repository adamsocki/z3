//
// Created by socki on 3/18/2025.
//

#ifndef Z3_MYIMGUI_H
#define Z3_MYIMGUI_H

#include "RenderManager.h"

namespace Zayn {

    struct MyImgui {

    };

    void ToggleImGuiVisibility(RenderManager* renderManager);
    void InitMyImgui(Zayn::RenderManager *pManager, Zayn::WindowManager *pWindowManager);
    void UpdateMyImgui(Game::CameraManager* cameraManager, RenderManager* renderManager, WindowManager* windowManager, InputManager* inputManager);
} // Zayn

#endif //Z3_MYIMGUI_H
