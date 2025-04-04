//
// Created by socki on 4/3/2025.
//

#include "PlayerController.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "../../Engine.h"
#include "../../tools/ComponentFactory.h"

namespace Game {

void InitPlayerController(PlayerController* component) {
    InitComponent((Component*)component);
    
    component->moveSpeed = 5.0f;
    component->jumpForce = 10.0f;
    component->airControl = 0.3f;
    component->moveInput = V2(0, 0);
    component->jumpRequested = false;
    component->canJump = true;
    component->jumpCooldown = 0.2f;
    component->jumpCooldownTimer = 0.0f;
}

void UpdatePlayerController(PlayerController* component, float deltaTime) {
    // Update jump cooldown
    if (!component->canJump) {
        component->jumpCooldownTimer -= deltaTime;
        if (component->jumpCooldownTimer <= 0) {
            component->canJump = true;
        }
    }
}

void ApplyJump(Zayn::Engine* engine, Zayn::EntityHandle playerEntity) {
    Zayn::ComponentStorage* componentStorage = &engine->componentFactory.componentStorage;
    
    // Get player components
    PhysicsComponent* physics = Zayn::FindComponentInArray(&componentStorage->physicsComponents, playerEntity);
    PlayerController* controller = Zayn::FindComponentInArray(&componentStorage->playerControllers, playerEntity);
    
    // Can only jump when grounded and jump cooldown is available
    if (physics && physics->isGrounded && controller && controller->canJump) {
        // Apply upward velocity for jump
        physics->velocity.y = controller->jumpForce;
        
        // Start jump cooldown
        controller->canJump = false;
        controller->jumpCooldownTimer = controller->jumpCooldown;
    }
}

void MovePlayer(Zayn::Engine* engine, Zayn::EntityHandle playerEntity, vec2 moveInput, float deltaTime) {
    Zayn::ComponentStorage* componentStorage = &engine->componentFactory.componentStorage;
    
    // Get player components
    PhysicsComponent* physics = Zayn::FindComponentInArray(&componentStorage->physicsComponents, playerEntity);
    TransformComponent* transform = Zayn::FindComponentInArray(&componentStorage->transformComponents, playerEntity);
    PlayerController* controller = Zayn::FindComponentInArray(&componentStorage->playerControllers, playerEntity);
    
    if (!physics || !transform || !controller) return;
    
    // Determine control factor based on grounded state
    float controlFactor = physics->isGrounded ? 1.0f : controller->airControl;
    
    // Calculate movement direction in world space
    vec3 movementDir = V3(moveInput.x, 0, moveInput.y) * controller->moveSpeed * controlFactor;
    
    // Preserve vertical velocity (for gravity and jumping)
    float yVelocity = physics->velocity.y;
    
    // Apply movement - directly set horizontal velocity for responsive controls
    physics->velocity.x = movementDir.x;
    physics->velocity.z = movementDir.z;
    physics->velocity.y = yVelocity; // Restore vertical velocity
}

} // Game