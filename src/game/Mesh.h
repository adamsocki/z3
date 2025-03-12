//
// Created by socki on 3/5/2025.
//

#ifndef Z3_MESH_H
#define Z3_MESH_H

#include <string>
#include <vulkan/vulkan_core.h>
#include <vector>
#include "../RenderManager.h"

namespace Game {

    struct Mesh {

        std::string name;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;
        uint32_t vertexCount;

        std::vector<Zayn::Vertex> vertices;
        std::vector<uint32_t> indices;

        bool isInitialized = false;

    };

} // Game

#endif //Z3_MESH_H
