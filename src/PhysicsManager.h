//
// Created by socki on 4/3/25.
//

#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include "math/math.h"

namespace Zayn {

struct Engine;

struct PhysicsManager {
    // Debug visualization settings
    bool showColliders = true; // Default on for testing
    bool showVelocities = true; // Default on for testing
    vec4 colliderColorSphere = V4(0.0f, 1.0f, 0.0f, 0.3f); // Green with transparency
    vec4 colliderColorBox = V4(0.0f, 0.0f, 1.0f, 0.3f);    // Blue with transparency
    vec4 colliderColorPlane = V4(1.0f, 1.0f, 0.0f, 0.3f);  // Yellow with transparency
    vec4 velocityColor = V4(1.0f, 0.0f, 0.0f, 1.0f);       // Red
};

void InitPhysicsManager();
void UpdatePhysicsManager(Engine* engine);
void RenderPhysicsDebugUI(Engine* engine);

} // Zayn

#endif //PHYSICSMANAGER_H
