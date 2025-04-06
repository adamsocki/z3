//
// Created by socki on 4/3/25.
//

#include "PhysicsManager.h"
#include "Engine.h"
#include "game/components/CollisionComponent.h"
#include "game/components/PhysicsComponent.h"
#include "math/math.h"
#include <imgui.h>
#include <fstream>
#include <vector>
#include "RenderManager.h"
#include "game/ModelPushConstant.h"
#include "game/UniformBufferObject.h"

#define GRAVITY_CONSTANT -9.81f

namespace Zayn {
    void InitPhysicsManager(Engine* engine, RenderManager* renderManager) {

        InitCollisionVisualization(engine, renderManager);

        // Init Debug Visualization for Collision if 3D colliders are enabled
        // if (engine->physicsManager.use3DColliders) {
        //     try {
        //         printf("Successfully initialized 3D collision visualization\n");
        //     } catch (const std::exception& e) {
        //         printf("Failed to initialize 3D collision visualization: %s\n", e.what());
        //         engine->physicsManager.use3DColliders = false; // Disable if initialization failed
        //     }
        // }

    }


void ApplyGravity(Zayn::Engine* engine) {
    Zayn::ComponentStorage* storage = &engine->componentFactory.componentStorage;

    // Get physics components
    Zayn::DynamicArray<Game::PhysicsComponent>* physicsComponents = &storage->physicsComponents;

    for (int i = 0; i < physicsComponents->count; i++) {
        Game::PhysicsComponent* physics_component = &(*physicsComponents)[i];

        if (!physics_component->useGravity || physics_component->isStatic) {
            continue;
        }

        if (physics_component->isGrounded) {
            physics_component->velocity.z = 0.0f;
            continue;
        }

        physics_component->acceleration.z = GRAVITY_CONSTANT;
    }

}

    void UpdateVelocities(Engine* engine, real32 deltaTime) {
        Zayn::ComponentStorage* storage = &engine->componentFactory.componentStorage;

        // Get physics components
        Zayn::DynamicArray<Game::PhysicsComponent>* physicsComponents = &storage->physicsComponents;

        for (int i = 0; i < physicsComponents->count; i++) {
            Game::PhysicsComponent* physics_component = &(*physicsComponents)[i];

            if (physics_component->isStatic) {
                continue;
            }

            if (physics_component->acceleration != V3(0,0,0))
            {
                physics_component->velocity = physics_component->velocity + (physics_component->acceleration * deltaTime);
            }

            physics_component->acceleration = V3(0,0,0);

        }
    }

    void UpdatePositions(Engine* engine, real32 deltaTime) {
        Zayn::ComponentStorage* storage = &engine->componentFactory.componentStorage;


        // Get physics components
        Zayn::DynamicArray<Game::PhysicsComponent>*     physicsComponents   = &storage->physicsComponents;
        Zayn::DynamicArray<Game::TransformComponent>*   transformComponents = &storage->transformComponents;

        for (int i = 0; i < physicsComponents->count; i++) {
            Game::PhysicsComponent* physics_component = &(*physicsComponents)[i];
            if (physics_component->isStatic) {
                continue;
            }

            Game::TransformComponent* transform_component = nullptr;
            for (uint32 j = 0; j < transformComponents->count; j++) {
                if ((*transformComponents)[j].owner == physics_component->owner) {
                    transform_component = &(*transformComponents)[j];
                    break;
                }
            }

            if (transform_component) {
                transform_component->position = transform_component->position + (physics_component->velocity * deltaTime);

                transform_component->transform = TRS(transform_component->position, FromEulerAngles(transform_component->rotation_euler_degrees), transform_component->scale);
            }
        }
    }

    bool CheckAABBCollision(Game::CollisionComponent* colliderA, Game::CollisionComponent* colliderB) {

        // Game::TransformComponent* transformA = nullptr;
        // for (int32 j = 0; j < transformComponents->count, j++) {
        //     if ((*transformComponents)[j].owner == collider)
        // }collider

        return true;
    }


    void DetectCollisions(Engine* engine) {
        ComponentStorage* storage = &engine->componentFactory.componentStorage;

        DynamicArray<Game::CollisionComponent>* collisionComponents = &storage->collisionComponents;
        DynamicArray<Game::TransformComponent>* transformComponents = &storage->transformComponents;

        for (int32 i = 0; i < collisionComponents->count; i++) {
            Game::CollisionComponent* colliderA = &(*collisionComponents)[i];

            Game::TransformComponent* transformA = nullptr;
            for (int32 j = 0; j < transformComponents->count; j++) {
                if ((*transformComponents)[j].owner == colliderA->owner) {
                    transformA = &(*transformComponents)[j];
                    break;
                }
            }

            if (!transformA) {
                continue; // Skip if no transform found
            }

            for (int32 j = 0; j < transformComponents->count; j++) {
                Game::CollisionComponent* colliderB = &(*collisionComponents)[j];

            }
        }


    }




    void StepPhysics(Zayn::Engine *engine) {
        TimeManager* timeManager = &engine->timeManager;
        real32 deltaTime = timeManager->time.deltaTime;
        ApplyGravity(engine);

        // Update velocities based on forces
        UpdateVelocities(engine, deltaTime);

    // Update positions based on velocities (do this BEFORE collision detection)
    // This way objects move first, then detect collisions, and stop without resetting position
        UpdatePositions(engine, deltaTime);

    // Detect collisions after moving objects
        DetectCollisions(engine);

    // Resolve detected collisions (just stop movement, don't adjust positions)
    // ResolveCollisions(engine);

    // Check which objects are grounded
    // CheckGrounded(engine);
}

void UpdatePhysicsManager(Zayn::Engine* engine) {

    StepPhysics(engine);

    // Initialize or cleanup collision visualization based on state
    // if (engine->physicsManager.use3DColliders &&
    //     engine->physicsManager.collisionPipeline == VK_NULL_HANDLE) {
    //     // Initialize if needed but not initialized
    //     try {
    //         InitCollisionVisualization(engine, &engine->renderManager);
    //         printf("Successfully initialized 3D collision visualization during update\n");
    //     } catch (const std::exception& e) {
    //         printf("Failed to initialize 3D collision visualization during update: %s\n", e.what());
    //         engine->physicsManager.use3DColliders = false; // Disable if initialization failed
    //     }
    // } else if ((!engine->physicsManager.use3DColliders) &&
    //             engine->physicsManager.collisionPipeline != VK_NULL_HANDLE) {
    //     // Clean up if initialized but not needed
    //     CleanupCollisionVisualization(engine, &engine->renderManager);
    // }
}

// Function to draw debug collision information
/*void RenderPhysicsDebugUI(Engine* engine) {
    // Skip if no physics debug is enabled
    if (!engine->physicsManager.showColliders && !engine->physicsManager.showVelocities) {
        printf("Debug visualization disabled\n");
        return;
    }
    
    printf("Debug visualization active! Colliders: %d, Velocities: %d\n", 
           engine->physicsManager.showColliders ? 1 : 0, 
           engine->physicsManager.showVelocities ? 1 : 0);

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) {
        printf("ERROR: ImGui background draw list is NULL!\n");
        return;
    }
    
    // Simple test - draw a big red circle in the center of the screen
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    float centerX = screenSize.x / 2.0f;
    float centerY = screenSize.y / 2.0f;
    
    printf("Drawing test circle at center (%.2f, %.2f) with size %d\n", centerX, centerY, 100);
    
    // Draw a large red circle as a test
    drawList->AddCircle(
        ImVec2(centerX, centerY),
        100.0f, // Large radius for visibility
        IM_COL32(255, 0, 0, 255), // Bright red
        32, // segments
        3.0f // thick line
    );
    
    // Draw a green rectangle too
    drawList->AddRect(
        ImVec2(centerX - 150, centerY - 150),
        ImVec2(centerX + 150, centerY + 150),
        IM_COL32(0, 255, 0, 255), // Bright green
        0.0f, // no rounding
        0, // no flags
        3.0f // thick line
    );
    
    ComponentStorage* storage = &engine->componentFactory.componentStorage;
    Game::CameraManager* cameraManager = &engine->cameraManager;
    
    // Get component arrays
    DynamicArray<Game::CollisionComponent>* collisionComponents = 
        &storage->collisionComponents;
    DynamicArray<Game::TransformComponent>* transformComponents = 
        &storage->transformComponents;
    DynamicArray<Game::PhysicsComponent>* physicsComponents = 
        &storage->physicsComponents;
    
    // Calculate screen size for scaling
    // ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    
    // Draw collision shapes
    if (engine->physicsManager.showColliders) {
        printf("Checking %d collision components\n", collisionComponents->count);
        
        // Iterate through all collision components
        for (uint32 i = 0; i < collisionComponents->count; i++) {
            Game::CollisionComponent* collider = &(*collisionComponents)[i];
            printf("Collision component %d: type=%d, owner=%d\n", i, collider->type, collider->owner);
            
            // Find transform for this collider
            Game::TransformComponent* transform = nullptr;
            for (uint32 j = 0; j < transformComponents->count; j++) {
                if ((*transformComponents)[j].owner == collider->owner) {
                    transform = &(*transformComponents)[j];
                    break;
                }
            }
            
            if (!transform) continue;
            
            // Convert world position to screen position
            vec3 worldPos = transform->position;
            printf("World position: (%.2f, %.2f, %.2f)\n", worldPos.x, worldPos.y, worldPos.z);
            vec2 screenPos = Game::WorldToScreen(cameraManager, worldPos);
            printf("Screen position: (%.2f, %.2f)\n", screenPos.x, screenPos.y);
            
            // Skip if behind camera
            if (screenPos.x < 0 || screenPos.y < 0 || 
                screenPos.x > screenSize.x || screenPos.y > screenSize.y) {
                continue;
            }
            
            // Draw based on collider type
            ImU32 color = IM_COL32(0, 255, 0, 100); // Default green

            if (collider->type == Game::COLLIDER_SPHERE) {
                // Calculate radius in screen space
                vec3 radiusPos = worldPos + V3(collider->radius, 0, 0);
                vec2 screenRadiusPos = Game::WorldToScreen(cameraManager, radiusPos);
                float screenRadius = Distance(screenPos, screenRadiusPos);
                
                // Minimum radius for visibility
                screenRadius = Max(5.0f, screenRadius);
                
                // Draw circle
                drawList->AddCircle(
                    ImVec2(screenPos.x, screenPos.y),
                    screenRadius,
                    color,
                    32, // segments
                    2.0f // thickness
                );
            }

            if (collider->type == Game::COLLIDER_BOX) {
                // Simplified box visualization (just the bounds)
                // In a real implementation, you'd transform all 8 corners
                
                // Get half-extents
                vec3 halfExtents = collider->size;
                
                // For simplicity, just draw the bounding box in screen space
                float screenSizeX = halfExtents.x * 40.0f; // Scaling factor for visibility
                float screenSizeY = halfExtents.y * 40.0f;
                
                drawList->AddRect(
                    ImVec2(screenPos.x - screenSizeX, screenPos.y - screenSizeY),
                    ImVec2(screenPos.x + screenSizeX, screenPos.y + screenSizeY),
                    IM_COL32(0, 0, 255, 100), // Blue for boxes
                    0.0f, // rounding
                    0, // flags
                    2.0f // thickness
                );
            }
            else if (collider->type == Game::COLLIDER_PLANE) {
                // Simplified plane visualization
                // In a real implementation, you'd draw a grid or a flat surface
                
                // Draw a line representing the normal
                vec3 normalEnd = worldPos + collider->normal * 2.0f;
                vec2 screenNormalEnd = Game::WorldToScreen(cameraManager, normalEnd);
                
                drawList->AddLine(
                    ImVec2(screenPos.x, screenPos.y),
                    ImVec2(screenNormalEnd.x, screenNormalEnd.y),
                    IM_COL32(255, 255, 0, 200), // Yellow for normals
                    2.0f // thickness
                );
                
                // Draw a small circle at the plane origin
                drawList->AddCircleFilled(
                    ImVec2(screenPos.x, screenPos.y),
                    5.0f,
                    IM_COL32(255, 255, 0, 200) // Yellow
                );
            }
        }
        
        // Draw collision results
        for (uint32 i = 0; i < engine->physicsFactory.collisionResults.count; i++) {
            CollisionResult* result = &engine->physicsFactory.collisionResults[i];
            if (!result->isValid) continue;
            
            // Convert collision point to screen position
            vec2 screenCollisionPoint = Game::WorldToScreen(cameraManager, result->collisionPoint);
            
            // Draw collision point
            drawList->AddCircleFilled(
                ImVec2(screenCollisionPoint.x, screenCollisionPoint.y),
                5.0f, // Small radius for collision point
                IM_COL32(255, 0, 0, 255) // Red
            );
            
            // Draw collision normal
            vec3 normalEnd = result->collisionPoint + result->collisionNormal * 1.0f;
            vec2 screenNormalEnd = Game::WorldToScreen(cameraManager, normalEnd);
            
            drawList->AddLine(
                ImVec2(screenCollisionPoint.x, screenCollisionPoint.y),
                ImVec2(screenNormalEnd.x, screenNormalEnd.y),
                IM_COL32(0, 255, 255, 255), // Cyan for normals
                2.0f // thickness
            );
        }
    }
    
    // Draw velocity vectors
    if (engine->physicsManager.showVelocities) {
        // Iterate through all physics components
        for (uint32 i = 0; i < physicsComponents->count; i++) {
            Game::PhysicsComponent* physics = &(*physicsComponents)[i];
            
            // Skip static objects or those with zero velocity
            if (physics->isStatic || LengthSq(physics->velocity) < 0.01f) {
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
            
            if (!transform) continue;
            
            // Convert world position to screen position
            vec3 worldPos = transform->position;
            printf("World position: (%.2f, %.2f, %.2f)\n", worldPos.x, worldPos.y, worldPos.z);
            vec2 screenPos = Game::WorldToScreen(cameraManager, worldPos);
            printf("Screen position: (%.2f, %.2f)\n", screenPos.x, screenPos.y);
            
            // Calculate endpoint of velocity vector
            float velocityScale = 0.5f; // Scale factor for visibility
            vec3 velocityEnd = worldPos + physics->velocity * velocityScale;
            vec2 screenVelocityEnd = Game::WorldToScreen(cameraManager, velocityEnd);
            
            // Draw velocity vector
            drawList->AddLine(
                ImVec2(screenPos.x, screenPos.y),
                ImVec2(screenVelocityEnd.x, screenVelocityEnd.y),
                IM_COL32(255, 0, 0, 255), // Red for velocity
                2.0f // thickness
            );
            
            // Draw arrowhead
            float arrowSize = 10.0f;
            vec2 dir = V2(screenVelocityEnd.x - screenPos.x, screenVelocityEnd.y - screenPos.y);
            dir = Normalize(dir);
            vec2 norm = V2(-dir.y, dir.x);
            
            drawList->AddTriangleFilled(
                ImVec2(screenVelocityEnd.x, screenVelocityEnd.y),
                ImVec2(screenVelocityEnd.x - dir.x * arrowSize + norm.x * arrowSize * 0.5f, 
                       screenVelocityEnd.y - dir.y * arrowSize + norm.y * arrowSize * 0.5f),
                ImVec2(screenVelocityEnd.x - dir.x * arrowSize - norm.x * arrowSize * 0.5f, 
                       screenVelocityEnd.y - dir.y * arrowSize - norm.y * arrowSize * 0.5f),
                IM_COL32(255, 0, 0, 255)
            );
        }
    }
}*/

// Reads a binary file (shader) into a vector
static std::vector<char> ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    
    return buffer;
}

// Creates a shader module from binary code
static VkShaderModule CreateShaderModule(const std::vector<char>& code, VkDevice device) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module");
    }
    
    return shaderModule;
}
/*
// Generate vertices and indices for a sphere
static void CreateSphereGeometry(Engine* engine, RenderManager* renderManager) {
    PhysicsManager* physicsManager = &engine->physicsManager;
    
    // Create a sphere with reasonable tessellation
    const int sectorCount = 36;
    const int stackCount = 18;
    const float radius = 1.0f; // Unit sphere - will be scaled by the shader
    
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    
    // Generate vertices
    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    
    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = M_PI / 2 - i * stackStep;  // starting from pi/2 to -pi/2
        float xy = radius * cosf(stackAngle);         // r * cos(u)
        float z = radius * sinf(stackAngle);          // r * sin(u)
        
        // Add (sectorCount+1) vertices per stack
        // The first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * sectorStep;       // starting from 0 to 2pi
            
            float x = xy * cosf(sectorAngle);         // r * cos(u) * cos(v)
            float y = xy * sinf(sectorAngle);         // r * cos(u) * sin(v)
            
            // Vertex position
            Vertex vertex{};
            vertex.pos = {x, y, z};
            
            // Normalized vertex normal
            vertex.normal = {x, y, z};
            
            // Texture coordinates
            vertex.texCoord = {(float)j / sectorCount, (float)i / stackCount};
            
            // Color (consistent for all vertices)
            vertex.color = {1.0f, 1.0f, 1.0f};
            
            vertices.push_back(vertex);
        }
    }
    
    // Generate indices
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;
        
        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            
            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    
    // Store the index count
    physicsManager->sphereIndexCount = indices.size();
    
    // Create vertex buffer
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    
    // Create staging buffer for vertex data
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(renderManager, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                 stagingBuffer, stagingBufferMemory);
    
    // Map memory and copy vertex data
    void* data;
    vkMapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory);
    
    // Create vertex buffer
    CreateBuffer(renderManager, bufferSize, 
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                 physicsManager->sphereVertexBuffer, 
                 physicsManager->sphereVertexBufferMemory);
    
    // Copy data to vertex buffer
    CopyBuffer(renderManager, stagingBuffer, physicsManager->sphereVertexBuffer, bufferSize);
    
    // Cleanup staging buffer
    vkDestroyBuffer(renderManager->vulkanData.vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, nullptr);
    
    // Create index buffer
    bufferSize = sizeof(indices[0]) * indices.size();
    
    // Create staging buffer for index data
    CreateBuffer(renderManager, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                 stagingBuffer, stagingBufferMemory);
    
    // Map memory and copy index data
    vkMapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory);
    
    // Create index buffer
    CreateBuffer(renderManager, bufferSize, 
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                 physicsManager->sphereIndexBuffer, 
                 physicsManager->sphereIndexBufferMemory);
    
    // Copy data to index buffer
    CopyBuffer(renderManager, stagingBuffer, physicsManager->sphereIndexBuffer, bufferSize);
    
    // Cleanup staging buffer
    vkDestroyBuffer(renderManager->vulkanData.vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, nullptr);
}
*/
// Generate vertices and indices for a box
static void CreateBoxGeometry(Engine* engine, RenderManager* renderManager) {
    PhysicsManager* physicsManager = &engine->physicsManager;
    
    // Create a unit cube - will be scaled by the shader
    std::vector<Vertex> vertices = {
        // Front face
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        
        // Back face
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        
        // Top face
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        
        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        
        // Right face
        {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        
        // Left face
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}}
    };
    
    std::vector<uint32_t> indices = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Top face
        8, 9, 10, 10, 11, 8,
        // Bottom face
        12, 13, 14, 14, 15, 12,
        // Right face
        16, 17, 18, 18, 19, 16,
        // Left face
        20, 21, 22, 22, 23, 20
    };
    
    // Store the index count
    physicsManager->boxIndexCount = indices.size();
    
    // Create vertex buffer
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    
    // Create staging buffer for vertex data
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(renderManager, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                 stagingBuffer, stagingBufferMemory);
    
    // Map memory and copy vertex data
    void* data;
    vkMapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory);
    
    // Create vertex buffer
    CreateBuffer(renderManager, bufferSize, 
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                 physicsManager->boxVertexBuffer, 
                 physicsManager->boxVertexBufferMemory);
    
    // Copy data to vertex buffer
    CopyBuffer(renderManager, stagingBuffer, physicsManager->boxVertexBuffer, bufferSize);
    
    // Cleanup staging buffer
    vkDestroyBuffer(renderManager->vulkanData.vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, nullptr);
    
    // Create index buffer
    bufferSize = sizeof(indices[0]) * indices.size();
    
    // Create staging buffer for index data
    CreateBuffer(renderManager, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                 stagingBuffer, stagingBufferMemory);
    
    // Map memory and copy index data
    vkMapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory);
    
    // Create index buffer
    CreateBuffer(renderManager, bufferSize, 
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                 physicsManager->boxIndexBuffer, 
                 physicsManager->boxIndexBufferMemory);
    
    // Copy data to index buffer
    CopyBuffer(renderManager, stagingBuffer, physicsManager->boxIndexBuffer, bufferSize);
    
    // Cleanup staging buffer
    vkDestroyBuffer(renderManager->vulkanData.vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, nullptr);
}

// Initialize the collision visualization pipeline and resources
void InitCollisionVisualization(Engine* engine, RenderManager* renderManager) {
    PhysicsManager* physicsManager = &engine->physicsManager;
    
    // Create geometry for colliders
    //CreateSphereGeometry(engine, renderManager);
    CreateBoxGeometry(engine, renderManager);
    
    // Compile shaders for collision visualization
    auto vertShaderCode = ReadFile("../src/render/shaders/compiled/vkShader_collision_vert.spv");
    auto fragShaderCode = ReadFile("../src/render/shaders/compiled/vkShader_collision_frag.spv");
    
    // Create shader modules
    physicsManager->collisionVertShaderModule = CreateShaderModule(vertShaderCode, renderManager->vulkanData.vkDevice);
    physicsManager->collisionFragShaderModule = CreateShaderModule(fragShaderCode, renderManager->vulkanData.vkDevice);
    
    // Set up shader stages
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = physicsManager->collisionVertShaderModule;
    vertShaderStageInfo.pName = "main";
    
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = physicsManager->collisionFragShaderModule;
    fragShaderStageInfo.pName = "main";
    
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
    
    // Set up dynamic states
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    // In InitCollisionVisualization
    physicsManager->collisionDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = renderManager->vulkanData.vkDescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &renderManager->vulkanData.vkDescriptorSetLayout;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        // Allocate descriptor set
        if (vkAllocateDescriptorSets(renderManager->vulkanData.vkDevice, &allocInfo,
                                     &physicsManager->collisionDescriptorSets[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets for collision rendering");
                                     }

        // Update with uniform buffer
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = renderManager->vulkanData.vkUniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(Game::UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = physicsManager->collisionDescriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(renderManager->vulkanData.vkDevice, 1, &descriptorWrite, 0, nullptr);
    }
    
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    
    // Set up vertex input
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    // Set up input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    // Set up viewport and scissor
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    
    // Set up rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_LINE; // Wireframe mode to see through colliders
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    
    // Set up multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    
    // Set up depth and stencil testing
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    
    // Set up color blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    
    // Set up push constants
    // We need to extend the push constant to include our color data
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(Game::ModelPushConstant) + sizeof(vec4); // Make room for model matrix and color
    
    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &renderManager->vulkanData.vkDescriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    
    if (vkCreatePipelineLayout(renderManager->vulkanData.vkDevice, &pipelineLayoutInfo, nullptr, &physicsManager->collisionPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout for collision visualization");
    }
    
    // Create the graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = physicsManager->collisionPipelineLayout;
    pipelineInfo.renderPass = renderManager->vulkanData.vkRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    
    if (vkCreateGraphicsPipelines(renderManager->vulkanData.vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &physicsManager->collisionPipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline for collision visualization");
    }
    
    printf("Collision visualization pipeline created successfully\n");
}

// Render colliders in 3D using our custom pipeline
void RenderColliders3D(Engine* engine, RenderManager* renderManager) {
    PhysicsManager* physicsManager = &engine->physicsManager;
    Game::CameraManager* cameraManager = &engine->cameraManager;
    
    // Skip if not enabled or not initialized
    if (!physicsManager->use3DColliders || physicsManager->collisionPipeline == VK_NULL_HANDLE) {
        return;
    }
    
    // Get current command buffer
    VkCommandBuffer commandBuffer = renderManager->vulkanData.vkCommandBuffers[renderManager->vulkanData.vkCurrentFrame];
    
    // Get component arrays
    ComponentStorage* storage = &engine->componentFactory.componentStorage;
    DynamicArray<Game::CollisionComponent>* collisionComponents = &storage->collisionComponents;
    DynamicArray<Game::TransformComponent>* transformComponents = &storage->transformComponents;
    
    // Bind our collision pipeline
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, physicsManager->collisionPipeline);
    
    // Set up viewport and scissor
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(renderManager->vulkanData.vkSwapChainExtent.width);
    viewport.height = static_cast<float>(renderManager->vulkanData.vkSwapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = renderManager->vulkanData.vkSwapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    
    // Bind descriptor sets
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                           physicsManager->collisionPipelineLayout, 0, 1,
                           &physicsManager->collisionDescriptorSets[renderManager->vulkanData.vkCurrentFrame],
                           0, nullptr);
    
    // Render each collider
    for (uint32 i = 0; i < collisionComponents->count; i++) {
        Game::CollisionComponent* collider = &(*collisionComponents)[i];
        
        // Find transform for this collider
        Game::TransformComponent* transform = nullptr;
        for (uint32 j = 0; j < transformComponents->count; j++) {
            if ((*transformComponents)[j].owner == collider->owner) {
                transform = &(*transformComponents)[j];
                break;
            }
        }
        
        if (!transform) continue;
        
        // Create model matrix for this collider
        mat4 modelMatrix = TRS(transform->position, FromEulerAngles(transform->rotation_euler_degrees), V3(1.0f, 1.0f, 1.0f));
        
        // Extended push constant with color
        struct {
            mat4 model;
            vec4 color;
        } pushConstant;
        
        // Set model matrix
        pushConstant.model = modelMatrix;

        VkBuffer vertexBuffers[1];
        VkDeviceSize offsets[] = {0};
        quaternion rotationQuat = IdentityQuaternion();

        vec3 rotation_rad = {};


        // Choose color based on collider type
        switch (collider->type) {
           /* case Game::ColliderType::COLLIDER_SPHERE:
                pushConstant.color = physicsManager->colliderColorSphere;
                
                // Scale model matrix based on sphere radius
                modelMatrix = TRS(transform->position, FromEulerAngles(transform->rotation_euler_degrees),V3(collider->radius, collider->radius, collider->radius));
                pushConstant.model = modelMatrix;
                
                // Bind the sphere vertex and index buffers
                vertexBuffers[0] = {physicsManager->sphereVertexBuffer};
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(commandBuffer, physicsManager->sphereIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
                
                // Push constants
                vkCmdPushConstants(commandBuffer, physicsManager->collisionPipelineLayout, 
                                  VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                                  0, sizeof(pushConstant), &pushConstant);
                
                // Draw
                vkCmdDrawIndexed(commandBuffer, physicsManager->sphereIndexCount, 1, 0, 0, 0);
                break;
                */
            case Game::ColliderType::COLLIDER_BOX:
                pushConstant.color = physicsManager->colliderColorBox;
                
                // Scale model matrix based on box size and properly apply rotation
                // Convert Euler angles from degrees to radians first
                rotation_rad = V3(ToRadians(transform->rotation_euler_degrees.x),
                                      ToRadians(transform->rotation_euler_degrees.y), 
                                      ToRadians(transform->rotation_euler_degrees.z));
                rotationQuat = FromEulerAngles(rotation_rad);
                modelMatrix = TRS(transform->position, rotationQuat, collider->size * 2.0f);
                pushConstant.model = modelMatrix;
                
                // Bind the box vertex and index buffers
                vertexBuffers[0] = physicsManager->boxVertexBuffer;
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(commandBuffer, physicsManager->boxIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
                
                // Push constants
                vkCmdPushConstants(commandBuffer, physicsManager->collisionPipelineLayout, 
                                  VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                                  0, sizeof(pushConstant), &pushConstant);
                
                // Draw
                vkCmdDrawIndexed(commandBuffer, physicsManager->boxIndexCount, 1, 0, 0, 0);
                break;
                
            case Game::ColliderType::COLLIDER_PLANE:
                // Plane visualization would be implemented similarly
                break;
            // default:
                // break;
        }
    }
}

// Clean up resources used for collision visualization
void CleanupCollisionVisualization(Engine* engine, RenderManager* renderManager) {
    PhysicsManager* physicsManager = &engine->physicsManager;
    VkDevice device = renderManager->vulkanData.vkDevice;
    
    // Destroy pipeline and pipeline layout
    if (physicsManager->collisionPipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(device, physicsManager->collisionPipeline, nullptr);
    }
    
    if (physicsManager->collisionPipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device, physicsManager->collisionPipelineLayout, nullptr);
    }
    
    // Destroy shader modules
    if (physicsManager->collisionVertShaderModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device, physicsManager->collisionVertShaderModule, nullptr);
    }
    
    if (physicsManager->collisionFragShaderModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device, physicsManager->collisionFragShaderModule, nullptr);
    }
    
    // Destroy buffers
    if (physicsManager->sphereVertexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, physicsManager->sphereVertexBuffer, nullptr);
    }
    
    if (physicsManager->sphereVertexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device, physicsManager->sphereVertexBufferMemory, nullptr);
    }
    
    if (physicsManager->sphereIndexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, physicsManager->sphereIndexBuffer, nullptr);
    }
    
    if (physicsManager->sphereIndexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device, physicsManager->sphereIndexBufferMemory, nullptr);
    }
    
    if (physicsManager->boxVertexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, physicsManager->boxVertexBuffer, nullptr);
    }
    
    if (physicsManager->boxVertexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device, physicsManager->boxVertexBufferMemory, nullptr);
    }
    
    if (physicsManager->boxIndexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, physicsManager->boxIndexBuffer, nullptr);
    }
    
    if (physicsManager->boxIndexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device, physicsManager->boxIndexBufferMemory, nullptr);
    }
    
    // Reset handles
    physicsManager->collisionPipeline = VK_NULL_HANDLE;
    physicsManager->collisionPipelineLayout = VK_NULL_HANDLE;
    physicsManager->collisionVertShaderModule = VK_NULL_HANDLE;
    physicsManager->collisionFragShaderModule = VK_NULL_HANDLE;
    physicsManager->sphereVertexBuffer = VK_NULL_HANDLE;
    physicsManager->sphereVertexBufferMemory = VK_NULL_HANDLE;
    physicsManager->sphereIndexBuffer = VK_NULL_HANDLE;
    physicsManager->sphereIndexBufferMemory = VK_NULL_HANDLE;
    physicsManager->boxVertexBuffer = VK_NULL_HANDLE;
    physicsManager->boxVertexBufferMemory = VK_NULL_HANDLE;
    physicsManager->boxIndexBuffer = VK_NULL_HANDLE;
    physicsManager->boxIndexBufferMemory = VK_NULL_HANDLE;
}

} // Zayn