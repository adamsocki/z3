//
// Created by socki on 4/3/25.
//

#include "PhysicsManager.h"
#include "Engine.h"
#include "game/components/CollisionComponent.h"
#include "game/components/PhysicsComponent.h"
#include "math/math.h"
#include <imgui.h>

namespace Zayn {

void InitPhysicsManager() {
    // Initialize physics manager
}

void UpdatePhysicsManager(Engine* engine) {
    // This function would be called from UpdateEngine
    // to handle any physics manager updates and debug visualization
}

// Function to draw debug collision information
void RenderPhysicsDebugUI(Engine* engine) {
    // Skip if no physics debug is enabled
    if (!engine->physicsManager.showColliders && !engine->physicsManager.showVelocities) {
        printf("Debug visualization disabled\n");
        return;
    }
    
    printf("Debug visualization active! Colliders: %d, Velocities: %d\n", 
           engine->physicsManager.showColliders ? 1 : 0, 
           engine->physicsManager.showVelocities ? 1 : 0);

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) {
        printf("ERROR: ImGui background draw list is NULL!\n");
        return;
    }
    
    // Simple test - draw a big red circle in the center of the screen
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    float centerX = screenSize.x / 2.0f;
    float centerY = screenSize.y / 2.0f;
    
    printf("Drawing test circle at center (%.2f, %.2f) with size %d\n", centerX, centerY, 100);
    
    // Draw a large red circle as a test
    drawList->AddCircle(
        ImVec2(centerX, centerY),
        100.0f, // Large radius for visibility
        IM_COL32(255, 0, 0, 255), // Bright red
        32, // segments
        3.0f // thick line
    );
    
    // Draw a green rectangle too
    drawList->AddRect(
        ImVec2(centerX - 150, centerY - 150),
        ImVec2(centerX + 150, centerY + 150),
        IM_COL32(0, 255, 0, 255), // Bright green
        0.0f, // no rounding
        0, // no flags
        3.0f // thick line
    );
    
    ComponentStorage* storage = &engine->componentFactory.componentStorage;
    Game::CameraManager* cameraManager = &engine->cameraManager;
    
    // Get component arrays
    DynamicArray<Game::CollisionComponent>* collisionComponents = 
        &storage->collisionComponents;
    DynamicArray<Game::TransformComponent>* transformComponents = 
        &storage->transformComponents;
    DynamicArray<Game::PhysicsComponent>* physicsComponents = 
        &storage->physicsComponents;
    
    // Calculate screen size for scaling
    // ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    
    // Draw collision shapes
    if (engine->physicsManager.showColliders) {
        printf("Checking %d collision components\n", collisionComponents->count);
        
        // Iterate through all collision components
        for (uint32 i = 0; i < collisionComponents->count; i++) {
            Game::CollisionComponent* collider = &(*collisionComponents)[i];
            printf("Collision component %d: type=%d, owner=%d\n", i, collider->type, collider->owner);
            
            // Find transform for this collider
            Game::TransformComponent* transform = nullptr;
            for (uint32 j = 0; j < transformComponents->count; j++) {
                if ((*transformComponents)[j].owner == collider->owner) {
                    transform = &(*transformComponents)[j];
                    break;
                }
            }
            
            if (!transform) continue;
            
            // Convert world position to screen position
            vec3 worldPos = transform->position;
            printf("World position: (%.2f, %.2f, %.2f)\n", worldPos.x, worldPos.y, worldPos.z);
            vec2 screenPos = Game::WorldToScreen(cameraManager, worldPos);
            printf("Screen position: (%.2f, %.2f)\n", screenPos.x, screenPos.y);
            
            // Skip if behind camera
            if (screenPos.x < 0 || screenPos.y < 0 || 
                screenPos.x > screenSize.x || screenPos.y > screenSize.y) {
                continue;
            }
            
            // Draw based on collider type
            ImU32 color = IM_COL32(0, 255, 0, 100); // Default green
            
            if (collider->type == Game::COLLIDER_SPHERE) {
                // Calculate radius in screen space
                vec3 radiusPos = worldPos + V3(collider->radius, 0, 0);
                vec2 screenRadiusPos = Game::WorldToScreen(cameraManager, radiusPos);
                float screenRadius = Distance(screenPos, screenRadiusPos);
                
                // Minimum radius for visibility
                screenRadius = Max(5.0f, screenRadius);
                
                // Draw circle
                drawList->AddCircle(
                    ImVec2(screenPos.x, screenPos.y),
                    screenRadius,
                    color,
                    32, // segments
                    2.0f // thickness
                );
            }
            else if (collider->type == Game::COLLIDER_BOX) {
                // Simplified box visualization (just the bounds)
                // In a real implementation, you'd transform all 8 corners
                
                // Get half-extents
                vec3 halfExtents = collider->size;
                
                // For simplicity, just draw the bounding box in screen space
                float screenSizeX = halfExtents.x * 40.0f; // Scaling factor for visibility
                float screenSizeY = halfExtents.y * 40.0f;
                
                drawList->AddRect(
                    ImVec2(screenPos.x - screenSizeX, screenPos.y - screenSizeY),
                    ImVec2(screenPos.x + screenSizeX, screenPos.y + screenSizeY),
                    IM_COL32(0, 0, 255, 100), // Blue for boxes
                    0.0f, // rounding
                    0, // flags
                    2.0f // thickness
                );
            }
            else if (collider->type == Game::COLLIDER_PLANE) {
                // Simplified plane visualization
                // In a real implementation, you'd draw a grid or a flat surface
                
                // Draw a line representing the normal
                vec3 normalEnd = worldPos + collider->normal * 2.0f;
                vec2 screenNormalEnd = Game::WorldToScreen(cameraManager, normalEnd);
                
                drawList->AddLine(
                    ImVec2(screenPos.x, screenPos.y),
                    ImVec2(screenNormalEnd.x, screenNormalEnd.y),
                    IM_COL32(255, 255, 0, 200), // Yellow for normals
                    2.0f // thickness
                );
                
                // Draw a small circle at the plane origin
                drawList->AddCircleFilled(
                    ImVec2(screenPos.x, screenPos.y),
                    5.0f,
                    IM_COL32(255, 255, 0, 200) // Yellow
                );
            }
        }
        
        // Draw collision results
        for (uint32 i = 0; i < engine->physicsFactory.collisionResults.count; i++) {
            CollisionResult* result = &engine->physicsFactory.collisionResults[i];
            if (!result->isValid) continue;
            
            // Convert collision point to screen position
            vec2 screenCollisionPoint = Game::WorldToScreen(cameraManager, result->collisionPoint);
            
            // Draw collision point
            drawList->AddCircleFilled(
                ImVec2(screenCollisionPoint.x, screenCollisionPoint.y),
                5.0f, // Small radius for collision point
                IM_COL32(255, 0, 0, 255) // Red
            );
            
            // Draw collision normal
            vec3 normalEnd = result->collisionPoint + result->collisionNormal * 1.0f;
            vec2 screenNormalEnd = Game::WorldToScreen(cameraManager, normalEnd);
            
            drawList->AddLine(
                ImVec2(screenCollisionPoint.x, screenCollisionPoint.y),
                ImVec2(screenNormalEnd.x, screenNormalEnd.y),
                IM_COL32(0, 255, 255, 255), // Cyan for normals
                2.0f // thickness
            );
        }
    }
    
    // Draw velocity vectors
    if (engine->physicsManager.showVelocities) {
        // Iterate through all physics components
        for (uint32 i = 0; i < physicsComponents->count; i++) {
            Game::PhysicsComponent* physics = &(*physicsComponents)[i];
            
            // Skip static objects or those with zero velocity
            if (physics->isStatic || LengthSq(physics->velocity) < 0.01f) {
                continue;
            }
            
            // Find transform for this entity
            Game::TransformComponent* transform = nullptr;
            for (uint32 j = 0; j < transformComponents->count; j++) {
                if ((*transformComponents)[j].owner == physics->owner) {
                    transform = &(*transformComponents)[j];
                    break;
                }
            }
            
            if (!transform) continue;
            
            // Convert world position to screen position
            vec3 worldPos = transform->position;
            printf("World position: (%.2f, %.2f, %.2f)\n", worldPos.x, worldPos.y, worldPos.z);
            vec2 screenPos = Game::WorldToScreen(cameraManager, worldPos);
            printf("Screen position: (%.2f, %.2f)\n", screenPos.x, screenPos.y);
            
            // Calculate endpoint of velocity vector
            float velocityScale = 0.5f; // Scale factor for visibility
            vec3 velocityEnd = worldPos + physics->velocity * velocityScale;
            vec2 screenVelocityEnd = Game::WorldToScreen(cameraManager, velocityEnd);
            
            // Draw velocity vector
            drawList->AddLine(
                ImVec2(screenPos.x, screenPos.y),
                ImVec2(screenVelocityEnd.x, screenVelocityEnd.y),
                IM_COL32(255, 0, 0, 255), // Red for velocity
                2.0f // thickness
            );
            
            // Draw arrowhead
            float arrowSize = 10.0f;
            vec2 dir = V2(screenVelocityEnd.x - screenPos.x, screenVelocityEnd.y - screenPos.y);
            dir = Normalize(dir);
            vec2 norm = V2(-dir.y, dir.x);
            
            drawList->AddTriangleFilled(
                ImVec2(screenVelocityEnd.x, screenVelocityEnd.y),
                ImVec2(screenVelocityEnd.x - dir.x * arrowSize + norm.x * arrowSize * 0.5f, 
                       screenVelocityEnd.y - dir.y * arrowSize + norm.y * arrowSize * 0.5f),
                ImVec2(screenVelocityEnd.x - dir.x * arrowSize - norm.x * arrowSize * 0.5f, 
                       screenVelocityEnd.y - dir.y * arrowSize - norm.y * arrowSize * 0.5f),
                IM_COL32(255, 0, 0, 255)
            );
        }
    }
}

} // Zayn