//
// Created by socki on 3/14/2025.
//

#include <iostream>
#include "CameraManager.h"

namespace Game {

    void InitCameraManager(CameraManager* cam, GLFWwindow* glfWwindow, Zayn::InputManager* inputManager)
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

        cam->cursorCaptured = false;
        cam->firstMouseCapture = true;

        cam->currentCursorMode = CURSOR_MODE_GAME;
        SetCursorMode(glfWwindow, inputManager, cam, cam->currentCursorMode);
    }

    void NormalizeVector(vec3* v)
    {
        float length = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
        if (length > 0.0001f) {
            v->x /= length;
            v->y /= length;
            v->z /= length;
        }
    }

    void CrossProduct(const vec3& a, const vec3& b, vec3* result)
    {
        result->x = a.y * b.z - a.z * b.y;
        result->y = a.z * b.x - a.x * b.z;
        result->z = a.x * b.y - a.y * b.x;
    }

    vec3 ScaleVector(const vec3& v, float scale)
    {
        vec3 result;
        result.x = v.x * scale;
        result.y = v.y * scale;
        result.z = v.z * scale;
        return result;
    }

    void ToggleCursorCapture(GLFWwindow* window, CameraManager* cam)
    {
        cam->cursorCaptured = !cam->cursorCaptured;
    }

    void EnterGameMode(Zayn::InputManager* inputManager)
    {
        Zayn::ResetMouseDelta(inputManager);
    }


    void SetCursorMode(GLFWwindow* glfwWindow, Zayn::InputManager* inputManager, CameraManager* cam, CursorMode newMode)
    {
//        if (cam->currentCursorMode == newMode)
//        {
//            return;
//        }

        CursorMode oldMode = cam->currentCursorMode;

        cam->currentCursorMode = newMode;
        cam->cursorModeJustChanged = true;

        // mode specific changes
        switch(cam->currentCursorMode)
        {
            case CURSOR_MODE_GAME:
                glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                EnterGameMode(inputManager);
                break;
            case CURSOR_MODE_UI:
                glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
            default:
                printf("ERROR: Attempted to set invalid cursor mode: %d\n", newMode);
                break;
        }


        printf("Cursor mode changed: %d -> %d\n", oldMode, cam->currentCursorMode);

    }

    void UpdateCameraManager(Zayn::WindowManager* windowManager, CameraManager* cam, Zayn::InputManager* inputManager, Zayn::TimeManager* timeManager)
    {
        switch (cam->currentCursorMode)
        {
            case CURSOR_MODE_GAME: {

                float moveSpeed = 15.0f * timeManager->time.deltaTime;

                // Get mouse delta and calculate rotation
                vec2 mouseDelta = GetMouseDelta(inputManager);

                // Apply mouse sensitivity
                float sensitivity = 0.1f;
                float yaw = mouseDelta.x * sensitivity;
                float pitch = -mouseDelta.y * sensitivity; // Invert Y for natural feel

                // Update camera angles
                cam->yaw -= yaw;
                cam->pitch += pitch;

                // Clamp pitch to avoid camera flipping
                if (cam->pitch > 89.0f)
                    cam->pitch = 89.0f;
                if (cam->pitch < -89.0f)
                    cam->pitch = -89.0f;

                // Calculate new orientation vectors based on yaw and pitch
                // Note: We're using a Z-up coordinate system
                float yawRad = cam->yaw * (3.14159f / 180.0f);
                float pitchRad = cam->pitch * (3.14159f / 180.0f);

                // Calculate front vector (this is the direction the camera is looking)
                cam->front.x = cos(yawRad) * cos(pitchRad);
                cam->front.y = sin(yawRad) * cos(pitchRad);
                cam->front.z = sin(pitchRad);
                NormalizeVector(&cam->front);

                // Recalculate right vector (perpendicular to front and world up)
                vec3 worldUp = {0.0f, 0.0f, 1.0f}; // Z-up world
                CrossProduct(cam->front, worldUp, &cam->right);
                NormalizeVector(&cam->right);

                // Recalculate up vector (perpendicular to front and right)
                CrossProduct(cam->right, cam->front, &cam->up);
                NormalizeVector(&cam->up);

                // Player Keyboard Movement (FPS style)
                if (InputHeld(inputManager->keyboard, Zayn::Input_W)) {
                    // Move forward in XY plane (FPS style)
                    vec3 forwardXY = cam->front;
                    forwardXY.z = 0.0f; // Zero out vertical component for typical FPS movement
                    NormalizeVector(&forwardXY);
                    cam->pos = cam->pos + ScaleVector(forwardXY, moveSpeed);
                }
                if (InputHeld(inputManager->keyboard, Zayn::Input_S)) {
                    // Move backward in XY plane
                    vec3 forwardXY = cam->front;
                    forwardXY.z = 0.0f;
                    NormalizeVector(&forwardXY);
                    cam->pos = cam->pos - ScaleVector(forwardXY, moveSpeed);
                }
                if (InputHeld(inputManager->keyboard, Zayn::Input_A)) {
                    // Strafe left
                    cam->pos = cam->pos - ScaleVector(cam->right, moveSpeed);
                }
                if (InputHeld(inputManager->keyboard, Zayn::Input_D)) {
                    // Strafe right
                    cam->pos = cam->pos + ScaleVector(cam->right, moveSpeed);
                }

                // Optional: Q/E for vertical movement (often used in free-cam modes)
                if (InputHeld(inputManager->keyboard, Zayn::Input_Q)) {
                    cam->pos.z += moveSpeed; // Move up in world space
                }
                if (InputHeld(inputManager->keyboard, Zayn::Input_E)) {
                    cam->pos.z -= moveSpeed; // Move down in world space
                }
                break;
            }

            case CURSOR_MODE_UI:
            {

                break;
            }

            default:
            {
                break;
            }
        }








        if (InputPressed(inputManager->keyboard, Zayn::Input_Tab)) {
            if (cam->currentCursorMode == CURSOR_MODE_GAME) {
                SetCursorMode(windowManager->glfwWindow, inputManager, cam, CURSOR_MODE_UI);
            } else {
                SetCursorMode(windowManager->glfwWindow, inputManager, cam, CURSOR_MODE_GAME);
            }
        }

    }
    
    vec2 WorldToScreen(CameraManager* cam, vec3 worldPos) {
        printf("WorldToScreen called with position (%.2f, %.2f, %.2f)\n", worldPos.x, worldPos.y, worldPos.z);
        
        // Print view-projection matrix for debugging
        printf("ViewProjection Matrix:\n");
        printf("%.2f %.2f %.2f %.2f\n", cam->viewProjection.m00, cam->viewProjection.m01, cam->viewProjection.m02, cam->viewProjection.m03);
        printf("%.2f %.2f %.2f %.2f\n", cam->viewProjection.m10, cam->viewProjection.m11, cam->viewProjection.m12, cam->viewProjection.m13);
        printf("%.2f %.2f %.2f %.2f\n", cam->viewProjection.m20, cam->viewProjection.m21, cam->viewProjection.m22, cam->viewProjection.m23);
        printf("%.2f %.2f %.2f %.2f\n", cam->viewProjection.m30, cam->viewProjection.m31, cam->viewProjection.m32, cam->viewProjection.m33);
        
        // Get current window size
        int width = 1920;  // Default
        int height = 1080; // Default
        
        // For immediate visibility, let's use a simple approach that directly maps world to screen
        // This is a temporary solution for debugging
        
        // The proper approach is to use the viewProjection matrix, but let's try a simpler approach first
        // This assumes the camera looks along the -Z axis (like in OpenGL)
        
        // Simple conversion - not mathematically correct but works as a temporary test
        float screenX = (width / 2.0f) + (worldPos.x * 100.0f); // Scale by 100 for better visibility
        float screenY = (height / 2.0f) - (worldPos.y * 100.0f); // Flip Y and scale
        
        printf("Simplified screen position: (%.2f, %.2f)\n", screenX, screenY);
        
        return V2(screenX, screenY);
        
        /*
        // Create a position in clip space by multiplying by view-projection matrix
        vec4 clipPos = cam->viewProjection * V4(worldPos.x, worldPos.y, worldPos.z, 1.0f);
        
        // Check if the point is behind the camera
        if (clipPos.w <= 0.0f) {
            return V2(-1.0f, -1.0f); // Return off-screen position
        }
        
        // Perform perspective division to get normalized device coordinates (NDC)
        vec3 ndcPos = V3(clipPos.x / clipPos.w, clipPos.y / clipPos.w, clipPos.z / clipPos.w);
        
        // Convert from NDC space [-1, 1] to screen space [0, width/height]
        // Assuming window dimensions are available in the CameraManager
        int width, height;
        
        // Get window size from aspect ratio - estimate
        // In a real implementation, you'd get this from the window manager
        width = 1920;  // Default width, should use actual window width
        height = static_cast<int>(width / cam->aspect);
        
        // Map from [-1, 1] to [0, width/height] and flip Y (screen Y is down)
        float screenX = (ndcPos.x + 1.0f) * 0.5f * static_cast<float>(width);
        float screenY = (1.0f - ndcPos.y) * 0.5f * static_cast<float>(height); // Flip Y
        
        return V2(screenX, screenY);
        */
    }
    
    vec3 ScreenToWorldRay(CameraManager* cam, vec2 screenPos) {
        // This function converts a 2D screen position to a 3D ray from camera
        // In a full implementation, you'd use this for picking/selection
        
        // For now, return a simplified implementation
        return Normalize(cam->front);
    }

} // Game