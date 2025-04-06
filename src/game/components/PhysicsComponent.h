//
// Created by socki on 4/3/2025.
//

#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include "Component.h"
#include "../../math/math.h"

namespace Game {

struct PhysicsComponent : Component {
    Zayn::EntityHandle owner;
    
    // Physics properties
    vec3 velocity = V3(0, 0, 0);
    vec3 acceleration = V3(0, 0, 0);
    // float mass = 1.0f;
    // float bounceiness = 0.0f;   // 0.0 = no bounce, 1.0 = perfect bounce
    // float friction = 1.0f;   // 0.0 = no friction, 1.0 = maximum friction
    bool useGravity = false;

    bool isStatic = true;      // Static objects don't move
    // bool isKinematic = false;   // Kinematic objects move but aren't affected by forces
    
    // Ground detection
    bool isGrounded = true;
    // float groundedThreshold = 0.1f; // How close to the ground to be considered "grounded"
};

void InitPhysicsComponent(PhysicsComponent* component);
void UpdatePhysicsComponent(PhysicsComponent* component);

} // Game

#endif //PHYSICSCOMPONENT_H