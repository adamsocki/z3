//
// Created by socki on 4/3/2025.
//

#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "Component.h"
#include "../../math/math.h"

namespace Game {

struct PlayerController : Component {
    Zayn::EntityHandle owner;
    
    // Movement settings
    float moveSpeed = 5.0f;
    float jumpForce = 10.0f;
    float airControl = 0.3f;   // Reduced control while in air
    
    // Input state
    vec2 moveInput = V2(0, 0);
    bool jumpRequested = false;
    
    // Jumping state
    bool canJump = true;
    float jumpCooldown = 0.2f;
    float jumpCooldownTimer = 0.0f;
};

void InitPlayerController(PlayerController* component);
void UpdatePlayerController(PlayerController* component, float deltaTime);

// Helper function for player jumping
void ApplyJump(Zayn::Engine* engine, Zayn::EntityHandle playerEntity);

// Helper function for player movement
void MovePlayer(Zayn::Engine* engine, Zayn::EntityHandle playerEntity, vec2 moveInput, float deltaTime);

} // Game

#endif //PLAYERCONTROLLER_H