//
// Created by socki on 3/14/2025.
//

#ifndef Z3_CAMERAMANAGER_H
#define Z3_CAMERAMANAGER_H

#include "../math/matrix.h"
#include "../InputManager.h"
#include "../TimeManager.h"

namespace Game {

    enum CameraType : uint8
    {
        CameraType_Perspective,
        CameraType_Orthographic,
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

// glm::vec3 pos;
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
    };

    void InitCameraManager(CameraManager* cam);
    void UpdateCameraManager(CameraManager* cam, Zayn::InputManager* inputManager, Zayn::TimeManager* timeManager);

} // Game

#endif //Z3_CAMERAMANAGER_H
