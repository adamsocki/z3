//
// Created by socki on 4/3/2025.
//

#include "PhysicsComponent.h"

namespace Game {

void InitPhysicsComponent(PhysicsComponent* component) {
   InitComponent((Component*)component);
    
    // component->velocity = V3(0, 0, 0);
    // component->acceleration = V3(0, 0, 0);
    // component->mass = 1.0f;
    // component->bounceiness = 0.3f;
    // component->useGravity = false;
    // component->isStatic = false;
    // component->isKinematic = false;
    // component->isGrounded = false;
    // component->groundedThreshold = 0.1f;
}

void UpdatePhysicsComponent(PhysicsComponent* component) {
    // This is mainly updated by the PhysicsFactory system
}

} // Game