//
// Created by socki on 3/14/2025.
//

#include <iostream>
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


        // Player Keyboard Movement
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


        vec2 mouseDelta = GetMouseDelta(inputManager);

        if(mouseDelta.x != 0)
        {
            std::cout << mouseDelta.x << std::endl;
            return;
        }
        float sensitivity = 0.1f;
        float yaw = mouseDelta.x * sensitivity;
        float pitch = -mouseDelta.y * sensitivity; // Invert Y for natural feel

        // Update camera angles
        cam->yaw += yaw;
        cam->pitch += pitch;

        // Clamp pitch to avoid camera flipping
        if (cam->pitch > 89.0f)
            cam->pitch = 89.0f;
        if (cam->pitch < -89.0f)
            cam->pitch = -89.0f;

        // Optional: Normalize yaw to keep it in a reasonable range
        while (cam->yaw > 360.0f)
            cam->yaw -= 360.0f;
        while (cam->yaw < 0.0f)
            cam->yaw += 360.0f;




    }

} // Game