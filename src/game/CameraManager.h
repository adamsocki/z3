//
// Created by socki on 3/14/2025.
//

#ifndef Z3_CAMERAMANAGER_H
#define Z3_CAMERAMANAGER_H

#include "../math/matrix.h"
#include "../InputManager.h"
#include "../TimeManager.h"
#include "../WindowManager.h"

namespace Game {

    enum CameraType : uint8
    {
        CameraType_Perspective,
        CameraType_Orthographic,
    };

    enum CursorMode {
        CURSOR_MODE_GAME,    // Cursor is hidden, captured for camera control
        CURSOR_MODE_UI,      // Cursor is visible, free to move
        CURSOR_MODE_COUNT
    };

    struct CameraManager {
        CameraType type;

        mat4 projection = Identity4();
        mat4 view = Identity4();

        mat4 viewProjection;

        real32 rotation;
        vec3 forwardDir;
        vec3 position;
        vec3 upDirection = V3(0.0f, 1.0f, 0.0f);

        vec3 pos;
        vec3 front;
        vec3 up;
        vec3 right;

        int32 frameCount;

        real32 vFOV;
        real32 aspect;
        real32 nearPlane;
        real32 farPlane;

// ortho
        real32 left;
        real32 right_;
        real32 top;
        real32 bottom;
        real32 nearDist;
        real32 farDist;

        real32 pitch;
        real32 yaw;
        real32 roll;

        real32 targetRotY;
        vec3 targetPos;
        real32 currentSpeed;
        real32 targetSpeed;
        real32 targetTurnSpeed;
        real32 currentTurnSpeed;
        real32 rotationSpeed;

        bool cursorCaptured;
        bool firstMouseCapture;
        CursorMode currentCursorMode;
        bool cursorModeJustChanged;

        real32 moveSpeed;

    };

    void InitCameraManager(CameraManager* cam, GLFWwindow* glfWwindow, Zayn::InputManager* inputManager);
    void UpdateCameraManager(Zayn::WindowManager* windowManager, CameraManager* cam, Zayn::InputManager* inputManager, Zayn::TimeManager* timeManager);
    void SetCursorMode(GLFWwindow* glfwWindow, Zayn::InputManager* inputManager, CameraManager* cam, CursorMode newMode);
} // Game

#endif //Z3_CAMERAMANAGER_H
