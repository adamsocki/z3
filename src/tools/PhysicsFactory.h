//
// Created by socki on 4/3/2025.
//

#ifndef PHYSICSFACTORY_H
#define PHYSICSFACTORY_H

#include "../math/math.h"
#include "../DynamicArray.h"
#include "ComponentFactory.h"
#include "../game/components/PhysicsComponent.h"
#include "../game/components/CollisionComponent.h"

namespace Zayn {

struct Engine;

// Store collision results for processing
struct CollisionResult {
    EntityHandle entityA;
    EntityHandle entityB;
    vec3 collisionPoint;
    vec3 collisionNormal;
    float penetrationDepth;
    bool isValid = false;
    bool forceCollision = false; // Used to mark collisions that were forced by distance-based detection
};

struct PhysicsFactory {
    vec3 gravity = V3(0.0f, 0.0f, -9.81f); // Standard gravity
    float fixedTimeStep = 1.0f / 60.0f;    // Physics updates at 60Hz
    float accumulatedTime = 0.0f;          // Used for fixed time step
    
    // Collision detection settings
    bool continuousCollisionDetection = false;
    int maxCollisionIterations = 4;         // Number of iterations for collision resolution
    
    // Ground detection
    float groundedRayLength = 0.2f;         // Length of ray to check for ground
    
    // Debug visualization
    bool showColliders = false;
    bool showVelocities = false;
    
    // Collision results storage
    DynamicArray<CollisionResult> collisionResults;
};

void InitPhysicsFactory(PhysicsFactory* physicsFactory, MemoryArena* arena);
void UpdatePhysicsFactory(Engine* engine, float deltaTime);

// Core physics functions
// void StepPhysics(Engine* engine, float timeStep);
// void ApplyGravity(Engine* engine, float deltaTime);
// void UpdateVelocities(Engine* engine, float deltaTime);
void UpdatePositions(Engine* engine, float deltaTime);
void DetectCollisions(Engine* engine);
void ResolveCollisions(Engine* engine);
void CheckGrounded(Engine* engine);

// Helper functions
void AddPhysicsComponentToEntity(Engine* engine, EntityHandle entity);
void AddCollisionComponentToEntity(Engine* engine, EntityHandle entity, Game::ColliderType type);

} // Zayn

#endif //PHYSICSFACTORY_H