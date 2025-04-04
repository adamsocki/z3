//
// Created by socki on 4/1/25.
//

#include "CollisionComponent.h"

namespace Game {

void InitCollisionComponent(CollisionComponent* component) {
    InitComponent((Component*)component);
    
    component->type = COLLIDER_SPHERE;
    component->radius = 1.0f;
    component->size = V3(1.0f, 1.0f, 1.0f);
    component->normal = V3(0, 0, 1);  // Z-up coordinate system
    component->offset = 0.0f;
    component->isTrigger = false;
    component->collisionLayer = 1;
    component->collisionMask = -1;
}

void UpdateCollisionComponent(CollisionComponent* component) {
    // This is mainly updated by the PhysicsFactory system

}

bool SphereSphereCollision(vec3 posA, float radiusA, vec3 posB, float radiusB, vec3* collisionPoint) {
    // Calculate distance between centers
    float distance = Distance(posA, posB);
    float radiusSum = radiusA + radiusB;
    
    // Check for collision
    if (distance < radiusSum) {
        // Calculate collision point if requested
        if (collisionPoint) {
            vec3 direction = Normalize(posB - posA);
            *collisionPoint = posA + direction * radiusA;
        }
        return true;
    }
    
    return false;
}

bool SphereBoxCollision(vec3 spherePos, float radius, vec3 boxPos, vec3 boxSize, vec3 boxRotation, vec3* collisionPoint) {
    // Calculate sphere position relative to box
    vec3 relativePos = spherePos - boxPos;
    
    // Find closest point on box to sphere center
    vec3 closestPoint;
    
    // Simple AABB check (no rotation handling yet)
    closestPoint.x = Clamp(relativePos.x, -boxSize.x, boxSize.x);
    closestPoint.y = Clamp(relativePos.y, -boxSize.y, boxSize.y);
    closestPoint.z = Clamp(relativePos.z, -boxSize.z, boxSize.z);
    
    // Calculate distance to closest point
    vec3 difference = relativePos - closestPoint;
    float distanceSq = LengthSq(difference);
    
    // Check for collision
    if (distanceSq < radius * radius) {
        // Calculate collision point if requested
        if (collisionPoint) {
            *collisionPoint = boxPos + closestPoint;
        }
        return true;
    }
    
    return false;
}

bool SpherePlaneCollision(vec3 spherePos, float radius, vec3 planeNormal, float planeOffset, vec3* collisionPoint) {
    // Calculate distance from sphere center to plane
    float distance = Dot(spherePos, planeNormal) - planeOffset;
    
    // Check for collision
    if (Abs(distance) < radius) {
        // Calculate collision point if requested
        if (collisionPoint) {
            *collisionPoint = spherePos - planeNormal * distance;
        }
        return true;
    }
    
    return false;
}

} // Game