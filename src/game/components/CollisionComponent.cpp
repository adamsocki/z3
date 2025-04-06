//
// Created by socki on 4/1/25.
//

#include "CollisionComponent.h"

namespace Game {



void InitCollisionComponent(CollisionComponent* component) {
    // InitComponent((Component*)component);
    
    // component->type = COLLIDER_BOX;
    // component->radius = 1.0f;
    // component->size = V3(1.0f, 1.0f, 1.0f);
    // component->normal = V3(0, 0, 1);  // Z-up coordinate system
    // component->offset = 0.0f;
    // component->isTrigger = false;
    // component->collisionLayer = 1;
    // component->collisionMask = -1;
}

void UpdateCollisionComponent(CollisionComponent* component) {
    // This is mainly updated by the PhysicsFactory system

}
//
// bool SphereSphereCollision(vec3 posA, float radiusA, vec3 posB, float radiusB, vec3* collisionPoint) {
//     // Calculate distance between centers
//     float distance = Distance(posA, posB);
//     float radiusSum = radiusA + radiusB;
//
//     // Check for collision
//     if (distance < radiusSum) {
//         // Calculate collision point if requested
//         if (collisionPoint) {
//             vec3 direction = Normalize(posB - posA);
//             *collisionPoint = posA + direction * radiusA;
//         }
//         return true;
//     }
//
//     return false;
// }


// Implementation for box collision with rotation support
// bool BoxBoxCollision(
//     vec3 posA, const vec3 &sizeA, const vec3 &rotA,
//     vec3 posB, const vec3 &sizeB, const vec3 &rotB,
//     vec3 *collisionPoint, vec3 *outCollisionNormal, float *outPenetrationDepth) {
// }
    // Create rotation quaternions from euler angles (converting from degrees to radians)
    // vec3 rotA_rad = V3(ToRadians(rotA.x), ToRadians(rotA.y), ToRadians(rotA.z));
    // vec3 rotB_rad = V3(ToRadians(rotB.x), ToRadians(rotB.y), ToRadians(rotB.z));
    // quaternion quatA = FromEulerAngles(rotA_rad);
    // quaternion quatB = FromEulerAngles(rotB_rad);
    //
    // // Convert quaternions to rotation matrices
    // mat4 rotMatrixA = Rotation4(quatA);
    // mat4 rotMatrixB = Rotation4(quatB);
    //
    // // Get axes from rotation matrices
    // vec3 axesA[3] = {
    //     V3(rotMatrixA.columns[0].x, rotMatrixA.columns[0].y, rotMatrixA.columns[0].z),
    //     V3(rotMatrixA.columns[1].x, rotMatrixA.columns[1].y, rotMatrixA.columns[1].z),
    //     V3(rotMatrixA.columns[2].x, rotMatrixA.columns[2].y, rotMatrixA.columns[2].z)
    // };
    //
    // vec3 axesB[3] = {
    //     V3(rotMatrixB.columns[0].x, rotMatrixB.columns[0].y, rotMatrixB.columns[0].z),
    //     V3(rotMatrixB.columns[1].x, rotMatrixB.columns[1].y, rotMatrixB.columns[1].z),
    //     V3(rotMatrixB.columns[2].x, rotMatrixB.columns[2].y, rotMatrixB.columns[2].z)
    // };
    //
    // // For better collision detection, we'll significantly increase the collision size
    // // This ensures we detect collisions early and prevent objects passing through each other
    // // This is a compromise between accuracy and performance
    //
    // // Calculate min and max points for both boxes considering rotation
    // // Use a larger expansion factor to ensure we catch collisions early
    // float rotationFactor = 1.5f; // Expand bounding box significantly to prevent interpenetration
    //
    // vec3 expandedSizeA = sizeA * rotationFactor;
    // vec3 expandedSizeB = sizeB * rotationFactor;
    //
    // vec3 minA = posA - expandedSizeA;
    // vec3 maxA = posA + expandedSizeA;
    // vec3 minB = posB - expandedSizeB;
    // vec3 maxB = posB + expandedSizeB;
    //
    // // Print debug info
    // printf("Box collision check: posA=(%.2f,%.2f,%.2f), posB=(%.2f,%.2f,%.2f)\n",
    //        posA.x, posA.y, posA.z, posB.x, posB.y, posB.z);
    //
    // // Check for overlap along all axes
    // bool hasOverlapX = !(maxA.x < minB.x || minA.x > maxB.x);
    // bool hasOverlapY = !(maxA.y < minB.y || minA.y > maxB.y);
    // bool hasOverlapZ = !(maxA.z < minB.z || minA.z > maxB.z);
    //
    // if (!hasOverlapX || !hasOverlapY || !hasOverlapZ) {
    //     printf("Box collision failed: overlapX=%d, overlapY=%d, overlapZ=%d\n",
    //            hasOverlapX ? 1 : 0, hasOverlapY ? 1 : 0, hasOverlapZ ? 1 : 0);
    //     return false; // No overlap, so no collision
    // }
    //
    // printf("Box collision detected!\n");
    //
    // // Calculate overlap on each axis
    // float overlapX = Min(maxA.x, maxB.x) - Max(minA.x, minB.x);
    // float overlapY = Min(maxA.y, maxB.y) - Max(minA.y, minB.y);
    // float overlapZ = Min(maxA.z, maxB.z) - Max(minA.z, minB.z);
    //
    // // Find the axis with the smallest overlap (minimum penetration axis)
    // float minOverlap = overlapX;
    // int axis = 0;
    //
    // if (overlapY < minOverlap) {
    //     minOverlap = overlapY;
    //     axis = 1;
    // }
    //
    // if (overlapZ < minOverlap) {
    //     minOverlap = overlapZ;
    //     axis = 2;
    // }
    //
    // // Calculate collision point (center of overlap)
    // if (collisionPoint) {
    //     vec3 overlapMin = V3(
    //         Max(minA.x, minB.x),
    //         Max(minA.y, minB.y),
    //         Max(minA.z, minB.z)
    //     );
    //
    //     vec3 overlapMax = V3(
    //         Min(maxA.x, maxB.x),
    //         Min(maxA.y, maxB.y),
    //         Min(maxA.z, maxB.z)
    //     );
    //
    //     *collisionPoint = (overlapMin + overlapMax) * 0.5f;
    // }
    //
    // // Set collision normal (from A to B)
    // if (outCollisionNormal) {
    //     vec3 normal = V3(0, 0, 0);
    //     normal[axis] = (posB[axis] > posA[axis]) ? 1.0f : -1.0f;
    //     *outCollisionNormal = normal;
    // }
    //
    // // Set penetration depth
    // if (outPenetrationDepth) {
    //     *outPenetrationDepth = minOverlap;
    // }
    //
    // return true;


// bool SphereBoxCollision(vec3 spherePos, float radius, vec3 boxPos, vec3 boxSize, vec3 boxRotation, vec3* collisionPoint) {
//     // Create rotation quaternion from euler angles (converting from degrees to radians)
//     vec3 boxRotation_rad = V3(ToRadians(boxRotation.x), ToRadians(boxRotation.y), ToRadians(boxRotation.z));
//     quaternion quat = FromEulerAngles(boxRotation_rad);
//
//     // Convert quaternion to rotation matrix
//     mat4 rotMatrix = Rotation4(quat);
//
//     // Calculate sphere position relative to box in local space
//     vec3 relativePos = spherePos - boxPos;
//
//     // Transform relative position to box's local space (inverse rotation)
//     vec3 localSpherePos;
//
//     // Transpose of rotation matrix is its inverse (assuming no scaling)
//     localSpherePos.x = Dot(relativePos, V3(rotMatrix.columns[0].x, rotMatrix.columns[1].x, rotMatrix.columns[2].x));
//     localSpherePos.y = Dot(relativePos, V3(rotMatrix.columns[0].y, rotMatrix.columns[1].y, rotMatrix.columns[2].y));
//     localSpherePos.z = Dot(relativePos, V3(rotMatrix.columns[0].z, rotMatrix.columns[1].z, rotMatrix.columns[2].z));
//
//     // Find closest point on box to sphere center in local space
//     vec3 closestPoint;
//     closestPoint.x = Clamp(localSpherePos.x, -boxSize.x, boxSize.x);
//     closestPoint.y = Clamp(localSpherePos.y, -boxSize.y, boxSize.y);
//     closestPoint.z = Clamp(localSpherePos.z, -boxSize.z, boxSize.z);
//
//     // Calculate distance to closest point in local space
//     vec3 localDifference = localSpherePos - closestPoint;
//     float distanceSq = LengthSq(localDifference);
//
//     // Check for collision
//     if (distanceSq < radius * radius) {
//         // Calculate collision point if requested (transform back to world space)
//         if (collisionPoint) {
//             // Transform closest point back to world space
//             vec3 worldClosestPoint;
//             worldClosestPoint.x = rotMatrix.columns[0].x * closestPoint.x + rotMatrix.columns[0].y * closestPoint.y + rotMatrix.columns[0].z * closestPoint.z;
//             worldClosestPoint.y = rotMatrix.columns[1].x * closestPoint.x + rotMatrix.columns[1].y * closestPoint.y + rotMatrix.columns[1].z * closestPoint.z;
//             worldClosestPoint.z = rotMatrix.columns[2].x * closestPoint.x + rotMatrix.columns[2].y * closestPoint.y + rotMatrix.columns[2].z * closestPoint.z;
//
//             *collisionPoint = boxPos + worldClosestPoint;
//         }
//         return true;
//     }
//
//     return false;
// }
//
// bool SpherePlaneCollision(vec3 spherePos, float radius, vec3 planeNormal, float planeOffset, vec3* collisionPoint) {
//     // Calculate distance from sphere center to plane
//     float distance = Dot(spherePos, planeNormal) - planeOffset;
//
//     // Check for collision
//     if (Abs(distance) < radius) {
//         // Calculate collision point if requested
//         if (collisionPoint) {
//             *collisionPoint = spherePos - planeNormal * distance;
//         }
//         return true;
//     }
//
//     return false;
// }

} // Game