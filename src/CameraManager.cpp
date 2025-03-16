//
// Created by socki on 3/14/2025.
//

#include "CameraManager.h"
namespace Game {

    void InitCameraManager(CameraManager* cam)
    {
        cam->rotationSpeed = 100.0f;
        cam->targetPos = V3(0.0f, 0.0f, 0.0f);

        cam->currentSpeed = 0;
        cam->targetSpeed = 20.0f;
        cam->targetTurnSpeed = 160.0f;

        cam->pos = V3(0, 0, 0.5f);
        cam->front = V3(-1, 0, 0);
        cam->up = V3(0, 0, 1);
        cam->right = V3(0, -1, 0);
    }

    void UpdateCameraManager(CameraManager* cam, Zayn::InputManager* inputManager, Zayn::TimeManager* timeManager)
    {



        if (InputHeld(inputManager->keyboard, Zayn::Input_W))
        {
            cam->pos = cam->pos + cam->front * 15.0f * timeManager->time.deltaTime;
        }
        if (InputHeld(inputManager->keyboard, Zayn::Input_S))
        {
            cam->pos = cam->pos - cam->front * 15.0f * timeManager->time.deltaTime;
        }

        if (InputHeld(inputManager->keyboard, Zayn::Input_Q))
        {
            cam->pos = cam->pos + cam->up * 15.0f * timeManager->time.deltaTime;
        }
        if (InputHeld(inputManager->keyboard, Zayn::Input_E))
        {
            cam->pos = cam->pos - cam->up * 15.0f * timeManager->time.deltaTime;
        }


    }

} // Game