//
// Created by socki on 4/1/25.
//

#ifndef COLLISIONCOMPONENT_H
#define COLLISIONCOMPONENT_H

#include "Component.h"
#include "../../math/math.h"

namespace Game {

enum ColliderType {
    // COLLIDER_SPHERE,
    COLLIDER_BOX,
    COLLIDER_PLANE  // For floors and other flat surfaces
};
    struct AABB {
        vec3 min;
        vec3 max;
    };

struct CollisionComponent : Component {
    Zayn::EntityHandle owner;

    // Collision shape properties
    ColliderType type = COLLIDER_BOX;
    vec3 size = V3(1.0f, 1.0f, 1.0f);  // Half-extents
    bool isTrigger = false;             // Trigger doesn't cause physical response


    // Sphere collider
    // float radius = 1.0f;

    // Box collider

    // Plane collider (for floors, walls)
    // vec3 normal = V3(0, 1, 0);          // Default points up (for floor)
    // float offset = 0.0f;                // Distance from origin

    // Collision flags
    // int32 collisionLayer = 1;           // Collision layer (for filtering)
    // int32 collisionMask = -1;           // Mask determines what layers to collide with
};

void InitCollisionComponent(CollisionComponent* component);
void UpdateCollisionComponent(CollisionComponent* component);

// Helper functions
// bool SphereSphereCollision(vec3 posA, float radiusA, vec3 posB, float radiusB, vec3* collisionPoint = nullptr);
// bool SphereBoxCollision(vec3 spherePos, float radius, vec3 boxPos, vec3 boxSize, vec3 boxRotation, vec3* collisionPoint = nullptr);
// bool SpherePlaneCollision(vec3 spherePos, float radius, vec3 planeNormal, float planeOffset, vec3* collisionPoint = nullptr);
// bool BoxBoxCollision(vec3 posA, const vec3 &sizeA, const vec3 &rotA, vec3 posB, const vec3 &sizeB, const vec3 &rotB, vec3 *collisionPoint, vec3
                     // *outCollisionNormal, float *outPenetrationDepth = nullptr);
} // Game

#endif //COLLISIONCOMPONENT_H