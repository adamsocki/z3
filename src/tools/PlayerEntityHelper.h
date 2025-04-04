/*//
// Created by socki on 4/3/2025.
//
#ifndef PLAYERENTITYHELPER_H
#define PLAYERENTITYHELPER_H


// #include "../Engine.h"
#include "../tools/EntityFactory.h"
#include "../tools/ComponentFactory.h"
#include "../game/entities/PlayerEntity.h"
#include "../game/components/PlayerController.h"
#include "../InputManager.h"

namespace Zayn {

// Helper function to add a PlayerController to an entity
inline void AddPlayerControllerToEntity(Engine* engine, EntityHandle entity) {
    ComponentStorage* componentStorage = &engine->componentFactory.componentStorage;
    
    // Check if entity already has a player controller component
    Game::PlayerController* existingComp = FindComponentInArray(&componentStorage->playerControllers, entity);
    if (existingComp) return; // Already has a component
    
    // Add new player controller component
    Game::PlayerController* newComp = AddComponent(&componentStorage->playerControllers, entity);
    if (newComp) {
        Game::InitPlayerController(newComp);
        newComp->owner = entity;
    }
}

// Create a complete player entity with all required components for physics
inline EntityHandle CreatePlayerEntity(Engine* engine, vec3 position, float radius = 1.0f) {
    // Create the player entity
    EntityHandle playerHandle = LE::CreateEntity(&engine->entityFactory, Game::EntityType::EntityType_Player);
    
    // Add required components
    Game::TransformComponent* transform = AddComponent(&engine->componentFactory.componentStorage.transformComponents, playerHandle);
    Game::InitComponent((Game::Component*)transform);
    transform->owner = playerHandle;
    transform->position = position;
    
    // Add physics component
    AddPhysicsComponentToEntity(engine, playerHandle);
    Game::PhysicsComponent* physics = FindComponentInArray(&engine->componentFactory.componentStorage.physicsComponents, playerHandle);
    if (physics) {
        physics->mass = 70.0f; // Average human mass in kg
        physics->bounceiness = 0.1f; // Small bounce for player
    }
    
    // Add collision component (sphere collider for player)
    AddCollisionComponentToEntity(engine, playerHandle, Game::COLLIDER_SPHERE);
    Game::CollisionComponent* collision = FindComponentInArray(&engine->componentFactory.componentStorage.collisionComponents, playerHandle);
    if (collision) {
        collision->radius = radius;
    }
    
    // Add player controller component
    AddPlayerControllerToEntity(engine, playerHandle);
    
    return playerHandle;
}

// Create a floor entity
inline EntityHandle CreateFloorEntity(Engine* engine, vec3 position, vec2 size) {
    // Create the floor entity
    EntityHandle floorHandle = CreateEntity(&engine->entityFactory, EntityType::FloorEntity_Type);
    
    // Add required components
    Game::TransformComponent* transform = AddComponent(&engine->componentFactory.componentStorage.transformComponents, floorHandle);
    Game::InitComponent((Game::Component*)transform);
    transform->owner = floorHandle;
    transform->position = position;
    
    // Add physics component (static)
    AddPhysicsComponentToEntity(engine, floorHandle);
    Game::PhysicsComponent* physics = FindComponentInArray(&engine->componentFactory.componentStorage.physicsComponents, floorHandle);
    if (physics) {
        physics->isStatic = true; // Floor doesn't move
        physics->useGravity = false;
    }
    
    // Add collision component (plane collider for floor)
    AddCollisionComponentToEntity(engine, floorHandle, Game::COLLIDER_PLANE);
    Game::CollisionComponent* collision = FindComponentInArray(&engine->componentFactory.componentStorage.collisionComponents, floorHandle);
    if (collision) {
        // Floor points up
        collision->normal = V3(0, 1, 0);
        collision->offset = Dot(position, collision->normal);
    }
    
    return floorHandle;
}

// Process player input and apply movement/jumping
inline void ProcessPlayerInput(Engine* engine, EntityHandle playerHandle, InputManager* input) {
    if (!input) return;
    
    // Get player controller
    Game::PlayerController* controller = FindComponentInArray(&engine->componentFactory.componentStorage.playerControllers, playerHandle);
    if (!controller) return;
    
    // Movement input from WASD
    vec2 moveInput = V2(0, 0);
    
    // Forward/Backward
    if (input->keyIsDown[GLFW_KEY_W]) moveInput.y += 1.0f;
    if (input->keyIsDown[GLFW_KEY_S]) moveInput.y -= 1.0f;
    
    // Left/Right
    if (InputHeld(&input->keyboard, Input_A) moveInput.x -= 1.0f;
    if (input->keyIsDown[GLFW_KEY_D]) moveInput.x += 1.0f;
    
    // Normalize if we have diagonal movement
    if (Length(moveInput) > 0.0f) {
        moveInput = Normalize(moveInput);
    }
    
    // Apply movement
    Game::MovePlayer(engine, playerHandle, moveInput, engine->timeManager.deltaTime);
    
    // Jump on space
    if (input->keyWasPressed[GLFW_KEY_SPACE]) {
        Game::ApplyJump(engine, playerHandle);
    }
    
    // Update controller state
    Game::UpdatePlayerController(controller, engine->timeManager.deltaTime);
}

} // Zayn

#endif //PLAYERENTITYHELPER_H

*/