//
// Created by socki on 4/3/25.
//

#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include "math/math.h"
#include <vulkan/vulkan.h>

namespace Zayn {

struct Engine;
struct RenderManager;

struct PhysicsManager {
    // Debug visualization settings
    // bool showColliders = true; // Default on for testing
    // bool showVelocities = true; // Default on for testing



    bool use3DColliders = false; // New option for 3D visualization
    // vec4 colliderColorSphere = V4(0.0f, 1.0f, 0.0f, 0.3f); // Green with transparency
    vec4 colliderColorBox = V4(0.0f, 0.0f, 1.0f, 0.3f);    // Blue with transparency
    // vec4 colliderColorPlane = V4(1.0f, 1.0f, 0.0f, 0.3f);  // Yellow with transparency
    vec4 velocityColor = V4(1.0f, 0.0f, 0.0f, 1.0f);       // Red
    
    // 3D collision visualization
    VkPipeline collisionPipeline = VK_NULL_HANDLE;
    VkPipelineLayout collisionPipelineLayout = VK_NULL_HANDLE;
    VkShaderModule collisionVertShaderModule = VK_NULL_HANDLE;
    VkShaderModule collisionFragShaderModule = VK_NULL_HANDLE;

    std::vector<VkDescriptorSet> collisionDescriptorSets;
    
    // Geometry for colliders
    VkBuffer sphereVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory sphereVertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer sphereIndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory sphereIndexBufferMemory = VK_NULL_HANDLE;
    uint32_t sphereIndexCount = 0;
    
    VkBuffer boxVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory boxVertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer boxIndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory boxIndexBufferMemory = VK_NULL_HANDLE;
    uint32_t boxIndexCount = 0;
};

void InitPhysicsManager(Engine* engine, RenderManager* renderManager);
void UpdatePhysicsManager(Engine* engine);
// void RenderPhysicsDebugUI(Engine* engine);
void InitCollisionVisualization(Engine* engine, RenderManager* renderManager);
void RenderColliders3D(Engine* engine, RenderManager* renderManager);
void CleanupCollisionVisualization(Engine* engine, RenderManager* renderManager);

} // Zayn

#endif //PHYSICSMANAGER_H
