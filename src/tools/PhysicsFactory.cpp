//
// Created by socki on 4/3/2025.
//

#include "PhysicsFactory.h"
#include "../Engine.h"
#include "../game/entities/Entity.h"

namespace Zayn {

void InitPhysicsFactory(PhysicsFactory* physicsFactory, MemoryArena* arena) {
    // Assuming model space has Z-up, apply gravity in the negative Z direction
    // physicsFactory->gravity = V3(0.0f, -9.81f, 0.0f);
    // physicsFactory->fixedTimeStep = 1.0f / 60.0f;
    // physicsFactory->accumulatedTime = 0.0f;
    // physicsFactory->maxCollisionIterations = 4;
    // physicsFactory->groundedRayLength = 0.2f;
    
    // Initialize collision results array
    physicsFactory->collisionResults = MakeDynamicArray<CollisionResult>(arena,  640);
}

void UpdatePhysicsFactory(Engine* engine, float deltaTime) {
    /*PhysicsFactory* physicsFactory = &engine->physicsFactory;
    
    // Accumulate time for fixed time step
    physicsFactory->accumulatedTime += deltaTime;
    
    // Step physics at fixed rate
    while (physicsFactory->accumulatedTime >= physicsFactory->fixedTimeStep) {
        StepPhysics(engine, physicsFactory->fixedTimeStep);
        physicsFactory->accumulatedTime -= physicsFactory->fixedTimeStep;
    }
    */
}

void StepPhysics(Engine* engine, float timeStep) {
    // Clear previous collision results
    // DynamicArray<CollisionResult>* collisionResults = &engine->physicsFactory.collisionResults;
    // DynamicArrayClear(collisionResults);
    
    // Apply forces (gravity, etc)
    // ApplyGravity(engine, timeStep);
    
    // Update velocities based on forces
    // UpdateVelocities(engine, timeStep);
    
    // Update positions based on velocities (do this BEFORE collision detection)
    // This way objects move first, then detect collisions, and stop without resetting position
    // UpdatePositions(engine, timeStep);
    
    // Detect collisions after moving objects
    // DetectCollisions(engine);
    
    // Resolve detected collisions (just stop movement, don't adjust positions)
    // ResolveCollisions(engine);
    
    // Check which objects are grounded
    // CheckGrounded(engine);
}

// void ApplyGravity(Engine* engine, float deltaTime) {
    /*
    PhysicsFactory* physicsFactory = &engine->physicsFactory;
    ComponentStorage* storage = &engine->componentFactory.componentStorage;
    
    // Get physics components array
    DynamicArray<Game::PhysicsComponent>* physicsComponents = 
        &storage->physicsComponents;
    
    // Apply gravity to each non-static physics component
    for (uint32 i = 0; i < physicsComponents->count; i++) {
        Game::PhysicsComponent* physics = &(*physicsComponents)[i];
        
        // Skip static and kinematic objects, or those with gravity disabled
        if (physics->isStatic || physics->isKinematic || !physics->useGravity) {
            continue;
        }
        
        // Skip objects that are grounded (for vertical gravity component only)
        if (physics->isGrounded ) {
            // Only cancel out vertical component of velocity for grounded objects
            physics->velocity.z = 0.0f;
            continue;
        }
        
        // Apply gravity as acceleration
        physics->acceleration = physics->acceleration + physicsFactory->gravity;
    }
    */
// }

// void UpdateVelocities(Engine* engine, float deltaTime) {
/*
    ComponentStorage* storage = &engine->componentFactory.componentStorage;
    
    // Get physics components array
    DynamicArray<Game::PhysicsComponent>* physicsComponents = 
        &storage->physicsComponents;
    
    // Update velocities based on acceleration
    for (uint32 i = 0; i < physicsComponents->count; i++) {
        Game::PhysicsComponent* physics = &(*physicsComponents)[i];
        
        // Skip static objects
        if (physics->isStatic) {
            continue;
        }
        
        // Apply acceleration to velocity (v = v0 + a*t)
        physics->velocity = physics->velocity + (physics->acceleration * deltaTime);
        
        // Reset acceleration for next frame
        physics->acceleration = V3(0, 0, 0);
    }
    */
// }

// void UpdatePositions(Engine* engine, float deltaTime) {


    /*ComponentStorage* storage = &engine->componentFactory.componentStorage;
    
    // Get component arrays
    DynamicArray<Game::PhysicsComponent>* physicsComponents = 
        &storage->physicsComponents;
    DynamicArray<Game::TransformComponent>* transformComponents = 
        &storage->transformComponents;
    
    // Update positions based on velocities
    for (uint32 i = 0; i < physicsComponents->count; i++) {
        Game::PhysicsComponent* physics = &(*physicsComponents)[i];
        
        // Skip static objects
        if (physics->isStatic) {
            continue;
        }
        
        // Find the matching transform component
        Game::TransformComponent* transform = nullptr;
        for (uint32 j = 0; j < transformComponents->count; j++) {
            if ((*transformComponents)[j].owner == physics->owner) {
                transform = &(*transformComponents)[j];
                break;
            }
        }
        
        if (transform) {
            // Update position (p = p0 + v*t)
            transform->position = transform->position + (physics->velocity * deltaTime);
            
            // Update transform matrix with proper rotation
            transform->transform = TRS(
                transform->position,
                FromEulerAngles(transform->rotation_euler_degrees),
                transform->scale
            );
            
            // Make sure the rotation is applied consistently for collision detection
        }
    }
    */
// }

// void DetectCollisions(Engine* engine) {
    /*ComponentStorage* storage = &engine->componentFactory.componentStorage;
    PhysicsFactory* physicsFactory = &engine->physicsFactory;
    EntityFactory* entityFactory = &engine->entityFactory;
    
    // Log information for debugging
    printf("DetectCollisions: Starting collision detection...\n");
    
    // Get component arrays
    DynamicArray<Game::CollisionComponent>* collisionComponents = 
        &storage->collisionComponents;
    DynamicArray<Game::TransformComponent>* transformComponents = 
        &storage->transformComponents;
    
    // Clear previous collision results
    DynamicArrayClear(&physicsFactory->collisionResults);
    
    // Basic N^2 collision detection between all pairs of colliders
    for (uint32 i = 0; i < collisionComponents->count; i++) {
        Game::CollisionComponent* colliderA = &(*collisionComponents)[i];
        
        // Find transform for collider A
        Game::TransformComponent* transformA = nullptr;
        for (uint32 k = 0; k < transformComponents->count; k++) {
            if ((*transformComponents)[k].owner == colliderA->owner) {
                transformA = &(*transformComponents)[k];
                break;
            }
        }
        
        if (!transformA) {
            continue; // Skip if no transform found
        }
        
        for (uint32 j = i + 1; j < collisionComponents->count; j++) {
            Game::CollisionComponent* colliderB = &(*collisionComponents)[j];
            
            // Skip if colliders are on non-colliding layers
            if ((colliderA->collisionLayer & colliderB->collisionMask) == 0 &&
                (colliderB->collisionLayer & colliderA->collisionMask) == 0) {
                continue;
            }
            
            // Find transform for collider B
            Game::TransformComponent* transformB = nullptr;
            for (uint32 k = 0; k < transformComponents->count; k++) {
                if ((*transformComponents)[k].owner == colliderB->owner) {
                    transformB = &(*transformComponents)[k];
                    break;
                }
            }
            
            if (!transformB) {
                continue; // Skip if no transform found
            }
            
            // Check for collision based on collider types
            bool collision = false;
            vec3 collisionPoint = V3(0, 0, 0);
            vec3 collisionNormal = V3(0, 0, 0);
            float penetrationDepth = 0.0f;
            
            // Sphere vs Sphere
            if (colliderA->type == Game::COLLIDER_SPHERE && colliderB->type == Game::COLLIDER_SPHERE) {
                collision = Game::SphereSphereCollision(
                    transformA->position, colliderA->radius,
                    transformB->position, colliderB->radius,
                    &collisionPoint);
                
                if (collision) {
                    collisionNormal = Normalize(transformA->position - transformB->position);
                    float distance = Distance(transformA->position, transformB->position);
                    penetrationDepth = (colliderA->radius + colliderB->radius) - distance;
                }
            }
            // Sphere vs Box
            else if (colliderA->type == Game::COLLIDER_SPHERE && colliderB->type == Game::COLLIDER_BOX) {
                collision = Game::SphereBoxCollision(
                    transformA->position, colliderA->radius,
                    transformB->position, colliderB->size,
                    transformB->rotation_euler_degrees,
                    &collisionPoint);
                
                if (collision) {
                    collisionNormal = Normalize(collisionPoint - transformB->position);
                    penetrationDepth = colliderA->radius - Distance(transformA->position, collisionPoint);
                }
            }
            // Box vs Sphere (swap A and B)
            else if (colliderA->type == Game::COLLIDER_BOX && colliderB->type == Game::COLLIDER_SPHERE) {
                collision = Game::SphereBoxCollision(
                    transformB->position, colliderB->radius,
                    transformA->position, colliderA->size,
                    transformA->rotation_euler_degrees,
                    &collisionPoint);
                
                if (collision) {
                    collisionNormal = Normalize(transformA->position - collisionPoint);
                    penetrationDepth = colliderB->radius - Distance(transformB->position, collisionPoint);
                }
            }
            // Sphere vs Plane
            else if (colliderA->type == Game::COLLIDER_SPHERE && colliderB->type == Game::COLLIDER_PLANE) {
                collision = Game::SpherePlaneCollision(
                    transformA->position, colliderA->radius,
                    colliderB->normal, colliderB->offset,
                    &collisionPoint);
                
                if (collision) {
                    collisionNormal = colliderB->normal;
                    float distance = Dot(transformA->position, colliderB->normal) - colliderB->offset;
                    penetrationDepth = colliderA->radius - Abs(distance);
                }
            }
            // Plane vs Sphere (swap A and B)
            else if (colliderA->type == Game::COLLIDER_PLANE && colliderB->type == Game::COLLIDER_SPHERE) {
                collision = Game::SpherePlaneCollision(
                    transformB->position, colliderB->radius,
                    colliderA->normal, colliderA->offset,
                    &collisionPoint);
                
                if (collision) {
                    collisionNormal = colliderA->normal * -1.0f; // Reverse normal for B
                    float distance = Dot(transformB->position, colliderA->normal) - colliderA->offset;
                    penetrationDepth = colliderB->radius - Abs(distance);
                }
            }
            // Box vs Box collision
            else if (colliderA->type == Game::COLLIDER_BOX && colliderB->type == Game::COLLIDER_BOX) {
                // Use a distance-based early check first to avoid missing collisions
                float distance = Distance(transformA->position, transformB->position);
                float combinedSize = Length(colliderA->size) + Length(colliderB->size);
                
                printf("Box-Box check: distance=%.2f, combinedSize=%.2f\n", distance, combinedSize);
                
                // Force a collision if objects are anywhere near each other
                // Using a very generous distance threshold to ensure we never miss collisions
                if (distance < combinedSize * 1.5f) {
                    printf("Box-Box: forcing collision due to close proximity\n");
                    collision = true;
                    collisionPoint = (transformA->position + transformB->position) * 0.5f;
                    collisionNormal = Normalize(transformB->position - transformA->position);
                    penetrationDepth = combinedSize * 0.5f - distance;
                    // Mark that this collision was forced and should get special handling
                    // result.forceCollision = true;
                } else {
                    // Regular AABB collision check
                    collision = Game::BoxBoxCollision(
                        transformA->position, colliderA->size, transformA->rotation_euler_degrees,
                        transformB->position, colliderB->size, transformB->rotation_euler_degrees,
                        &collisionPoint, &collisionNormal, &penetrationDepth);
                    
                    // Don't modify the object's position when it collides - this causes "teleportation"
                    // Just set a flag to indicate we should stop moving
                    if (collision) {
                        printf("Regular BoxBoxCollision detected - will stop movement without position correction\n");
                    }
                }
            }

            // If collision detected, store result
            if (collision) {
                CollisionResult result;
                result.entityA = colliderA->owner;
                result.entityB = colliderB->owner;
                result.collisionPoint = collisionPoint;
                result.collisionNormal = collisionNormal;
                result.penetrationDepth = penetrationDepth;
                result.isValid = true;
                
                // Add to collision results array
                PushBack(&physicsFactory->collisionResults, result);
            }
        }
    }*/
// }

void ResolveCollisions(Engine* engine) {
    /*
    ComponentStorage* storage = &engine->componentFactory.componentStorage;
    PhysicsFactory* physicsFactory = &engine->physicsFactory;
    EntityFactory* entityFactory = &engine->entityFactory;
    
    // Get component arrays
    DynamicArray<Game::PhysicsComponent>* physicsComponents = 
        &storage->physicsComponents;
    DynamicArray<Game::TransformComponent>* transformComponents = 
        &storage->transformComponents;
    DynamicArray<Game::CollisionComponent>* collisionComponents = 
        &storage->collisionComponents;
    
    // Iterative collision resolution
    for (int iteration = 0; iteration < physicsFactory->maxCollisionIterations; iteration++) {
        // Process each collision result
        for (uint32 i = 0; i < physicsFactory->collisionResults.count; i++) {
            CollisionResult* result = &physicsFactory->collisionResults[i];
            
            if (!result->isValid) {
                continue;
            }
            
            // Find physics components for both entities
            Game::PhysicsComponent* physicsA = nullptr;
            Game::PhysicsComponent* physicsB = nullptr;
            
            for (uint32 j = 0; j < physicsComponents->count; j++) {
                Game::PhysicsComponent* physics = &(*physicsComponents)[j];
                
                if (physics->owner == result->entityA) {
                    physicsA = physics;
                }
                else if (physics->owner == result->entityB) {
                    physicsB = physics;
                }
                
                if (physicsA && physicsB) {
                    break;
                }
            }
            
            // Find collision components for both entities
            Game::CollisionComponent* colliderA = nullptr;
            Game::CollisionComponent* colliderB = nullptr;
            
            for (uint32 j = 0; j < collisionComponents->count; j++) {
                Game::CollisionComponent* collider = &(*collisionComponents)[j];
                
                if (collider->owner == result->entityA) {
                    colliderA = collider;
                }
                else if (collider->owner == result->entityB) {
                    colliderB = collider;
                }
                
                if (colliderA && colliderB) {
                    break;
                }
            }
            
            // Skip if required components not found
            if (!physicsA || !physicsB || !colliderA || !colliderB) {
                continue;
            }
            
            // Skip collision resolution for triggers
            if (colliderA->isTrigger || colliderB->isTrigger) {
                // Handle trigger events here if needed
                continue;
            }
            
            // Skip if both objects are static
            if (physicsA->isStatic && physicsB->isStatic) {
                continue;
            }
            
            // Find transforms for position correction
            Game::TransformComponent* transformA = nullptr;
            Game::TransformComponent* transformB = nullptr;
            
            for (uint32 j = 0; j < transformComponents->count; j++) {
                Game::TransformComponent* transform = &(*transformComponents)[j];
                
                if (transform->owner == result->entityA) {
                    transformA = transform;
                }
                else if (transform->owner == result->entityB) {
                    transformB = transform;
                }
                
                if (transformA && transformB) {
                    break;
                }
            }
            
            if (!transformA || !transformB) {
                continue;
            }
            
            // Position correction to resolve penetration - increase correction strength
            float totalInverseMass = 0.0f;
            
            if (!physicsA->isStatic) {
                totalInverseMass += 1.0f / physicsA->mass;
            }
            
            if (!physicsB->isStatic) {
                totalInverseMass += 1.0f / physicsB->mass;
            }
            
            // Skip if total inverse mass is zero (both objects are static/immovable)
            if (totalInverseMass <= 0.0f) {
                continue;
            }
            
            // Disable all position correction - objects should stop exactly where they are
            
            // Only apply minimal position correction if needed to prevent visual artifacts
            if (false) { // Completely disable all position correction
                // Only apply minimal correction to prevent visual intersections
                float correctionAmount = result->penetrationDepth * 0.1f; // Minimal correction
                
                vec3 correction = result->collisionNormal * correctionAmount;
                
                // Apply position correction but only for forced collisions
                if (!physicsA->isStatic) {
                    transformA->position = transformA->position + correction * 0.5f;
                }
                
                if (!physicsB->isStatic) {
                    transformB->position = transformB->position - correction * 0.5f;
                }
            }
            
            // Always update transform matrices to ensure rendering matches physics state
            if (!physicsA->isStatic) {
                transformA->transform = TRS(
                    transformA->position,
                    FromEulerAngles(transformA->rotation_euler_degrees),
                    transformA->scale
                );
            }
            
            if (!physicsB->isStatic) {
                transformB->transform = TRS(
                    transformB->position,
                    FromEulerAngles(transformB->rotation_euler_degrees),
                    transformB->scale
                );
            }

            if (physicsA && !physicsA->isStatic) {
                physicsA->isGrounded = true;
                physicsA->velocity = V3(0, 0, 0);
            }
            
            if (physicsB && !physicsB->isStatic) {
                physicsB->isGrounded = true;
                physicsB->velocity = V3(0, 0, 0);
            }
            
            // Skip any additional physics processing for this collision
            continue;
            
            // Calculate relative velocity
            // vec3 relativeVelocity = physicsB->velocity - physicsA->velocity;
            //
            // // Calculate velocity along the normal
            // float velocityAlongNormal = Dot(relativeVelocity, result->collisionNormal);
            //
            // // Only resolve if objects are moving toward each other
            // if (velocityAlongNormal > 0) {
            //     continue;
            // }
            //
            // // Disable restitution completely - no bouncing at all
            // float restitution = 0.0f;
            //
            // // Calculate impulse scalar
            // float j = -(1.0f + restitution) * velocityAlongNormal;
            // j /= totalInverseMass;
            //
            // // Apply impulse
            // vec3 impulse = result->collisionNormal * j;
            //
            // if (!physicsA->isStatic && !physicsA->isKinematic) {
            //     physicsA->velocity = physicsA->velocity - impulse / physicsA->mass;
            //
            //     // Any collision will set the object as grounded and stop movement
            //     physicsA->isGrounded = true;
            //     physicsA->velocity = V3(0, 0, 0);
            //
            //     // We've already set velocity to zero and marked as grounded
            //     // No additional friction needed
            // }
            //
            // if (!physicsB->isStatic && !physicsB->isKinematic) {
            //     physicsB->velocity = physicsB->velocity + impulse / physicsB->mass;
            //
            //     // Any collision will set the object as grounded and stop movement
            //     physicsB->isGrounded = true;
            //     physicsB->velocity = V3(0, 0, 0);
            //
            //     // We've already set velocity to zero and marked as grounded
            //     // No additional friction needed
            // }
        }
    }
    */
}

void CheckGrounded(Engine* engine) {
    /*
    ComponentStorage* storage = &engine->componentFactory.componentStorage;
    PhysicsFactory* physicsFactory = &engine->physicsFactory;
    EntityFactory* entityFactory = &engine->entityFactory;
    
    // Get component arrays
    DynamicArray<Game::PhysicsComponent>* physicsComponents = 
        &storage->physicsComponents;
    DynamicArray<Game::TransformComponent>* transformComponents = 
        &storage->transformComponents;
    DynamicArray<Game::CollisionComponent>* collisionComponents = 
        &storage->collisionComponents;
    
    // Clear previous grounded states
    for (uint32 i = 0; i < physicsComponents->count; i++) {
        Game::PhysicsComponent* physics = &(*physicsComponents)[i];
        
        // Don't change static objects
        if (physics->isStatic) {
            continue;
        }
        
        // Mark as not grounded by default
        physics->isGrounded = false;
    }
    
    // Check each physics component for ground contact
    for (uint32 i = 0; i < physicsComponents->count; i++) {
        Game::PhysicsComponent* physics = &(*physicsComponents)[i];
        
        // Skip static objects - they're always "grounded"
        if (physics->isStatic) {
            physics->isGrounded = true;
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
        
        if (!transform) {
            continue;
        }
        
        // Default to not grounded
        physics->isGrounded = false;
        
        // Check all collision components for possible ground
        for (uint32 j = 0; j < collisionComponents->count; j++) {
            Game::CollisionComponent* potentialGround = &(*collisionComponents)[j];
            
            // Skip self
            if (potentialGround->owner == physics->owner) {
                continue;
            }
            
            // Find transform for ground entity
            Game::TransformComponent* groundTransform = nullptr;
            for (uint32 k = 0; k < transformComponents->count; k++) {
                if ((*transformComponents)[k].owner == potentialGround->owner) {
                    groundTransform = &(*transformComponents)[k];
                    break;
                }
            }
            
            if (!groundTransform) {
                continue;
            }
            
            // Special handling for plane colliders (good for floors)
            if (potentialGround->type == Game::COLLIDER_PLANE) {
                // Check if plane normal points upward in Z direction (is a floor)
                if (potentialGround->normal.z > 0.7f) {
                    // Calculate distance from object to plane
                    float distance = Dot(transform->position, potentialGround->normal) - potentialGround->offset;
                    
                    // Check if within ground detection threshold
                    if (distance < physics->groundedThreshold) {
                        physics->isGrounded = true;
                        break;
                    }
                }
            }
            // Special case for sphere and box colliders
            else {
                // For non-plane colliders, check if object is directly above
                // Get entity objects for more detailed checking
                Game::Entity* entity = static_cast<Game::Entity*>(GetEntity(entityFactory, physics->owner));
                Game::Entity* groundEntity = static_cast<Game::Entity*>(GetEntity(entityFactory, potentialGround->owner));
                
                if (entity && groundEntity) {
                    // Check if the entity's bottom is close to the ground entity's top
                    // This is a simplified check - could be improved for more accurate ground detection
                    float entityBottom = transform->position.z;  // Z is the vertical axis in the model space
                    float groundTop = groundTransform->position.z;
                    
                    // Adjust based on collider types
                    if (potentialGround->type == Game::COLLIDER_SPHERE) {
                        groundTop += potentialGround->radius;
                    }
                    else if (potentialGround->type == Game::COLLIDER_BOX) {
                        groundTop += potentialGround->size.z;  // Use Z component for vertical size
                    }
                    
                    // Check if within ground detection threshold
                    if (Abs(entityBottom - groundTop) < physics->groundedThreshold) {
                        physics->isGrounded = true;
                        break;
                    }
                }
            }
        }
    }
    */
}

// Helper function to add a physics component to an entity
void AddPhysicsComponentToEntity(Engine* engine, EntityHandle entity) {

    ComponentStorage* storage = &engine->componentFactory.componentStorage;
    
    // Create new physics component
    Game::PhysicsComponent physics;
    InitPhysicsComponent(&physics);
    
    // Set owner
    physics.owner = entity;
    
    // Add to component storage
    PushBack(&storage->physicsComponents, physics);
    
    // Update entity with component reference
    Game::Entity* entityObj = static_cast<Game::Entity*>(GetEntity(&engine->entityFactory, entity));
    if (entityObj) {
        // Could add component reference to entity if needed
    }

}

// Helper function to add a collision component to an entity
void AddCollisionComponentToEntity(Engine* engine, EntityHandle entity, Game::ColliderType type) {
    ComponentStorage* storage = &engine->componentFactory.componentStorage;
    
    // Create new collision component
    Game::CollisionComponent collision;
    Game::InitCollisionComponent(&collision);
    
    // Set owner and type
    collision.owner = entity;
    collision.type = type;
    
    // Add to component storage
    PushBack(&storage->collisionComponents, collision);
    
    // Update entity with component reference
    // Game::Entity* entityObj = static_cast<Game::Entity*>(GetEntity(&engine->entityFactory, entity));
    // if (entityObj) {
    //     // Could add component reference to entity if needed
    // }
}

} // Zayn