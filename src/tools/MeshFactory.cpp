//
// Created by socki on 3/5/2025.
//
#include <map>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "MeshFactory.h"
#include "../Engine.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include "../include/tiny_obj_loader.h"




namespace std {
    template<> struct hash<Zayn::Vertex> {
        size_t operator()(Zayn::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

namespace Zayn {

    std::string GetModelPath(const std::string& filename) {
    #ifdef WIN32
        return "../src/render/models/" + filename;
    #elif __APPLE__
        return "/Users/socki/dev/z3/src/render/models/" + filename;
    #endif
    }

    void CreateIndexBuffer(RenderManager* renderManager, std::vector<uint32_t> indices, VkBuffer* indexBuffer, VkDeviceMemory* indexBufferMemory)
    {
        if (indices.empty())
        {
            return;
        }
        VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(renderManager, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory);

        CreateBuffer(renderManager,bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *indexBuffer, *indexBufferMemory);

        CopyBuffer(renderManager, stagingBuffer, *indexBuffer, bufferSize);

        vkDestroyBuffer(renderManager->vulkanData.vkDevice, stagingBuffer, nullptr);
        vkFreeMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, nullptr);
    }

    void CreateVertexBuffer(RenderManager* renderManager, std::vector<Vertex>& vertices, VkBuffer* vertexBuffer, VkDeviceMemory* vertexBufferMemory)
    {
        if (vertices.empty())
        {
            return;
        }

        VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();
        // STAGING BUFFER - CPU accessible memory to upload the data from the vertex array to.
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(renderManager, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory);

        CreateBuffer(renderManager, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *vertexBuffer, *vertexBufferMemory);

        CopyBuffer(renderManager,stagingBuffer, *vertexBuffer, bufferSize);

        vkDestroyBuffer(renderManager->vulkanData.vkDevice, stagingBuffer, nullptr);
        vkFreeMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, nullptr);
    }


    void LoadModel( std::string modelPath, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str()))
        {
            throw std::runtime_error(warn + err);
        }
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{};

                // Load the vertex position directly (without coordinate system change)
                vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2] };

                vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };

                vertex.color = { 0.3f, 1.0f, 0.6f };
                
                // Set default normal (pointing up in Y direction)
                vertex.normal = { 0.0f, 1.0f, 0.0f };

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices->size());
                    vertices->push_back(vertex);
                }

                indices->push_back(uniqueVertices[vertex]);
            }
        }
    }


    void MakeMesh(Engine* engine, MeshCreateInfo* info)
    {
        RenderManager* renderManager = &engine->renderManager;
        Game::Mesh mesh ={};
        mesh.path = info->path;
        mesh.name = info->name;
        LoadModel(GetModelPath(mesh.path), &mesh.vertices, &mesh.indices);
        CreateVertexBuffer(renderManager, mesh.vertices, &mesh.vertexBuffer, &mesh.vertexBufferMemory);
        CreateIndexBuffer(renderManager, mesh.indices, &mesh.indexBuffer, &mesh.indexBufferMemory);

        uint32_t meshIndex = PushBack(&engine->meshFactory.meshes, mesh);
        Game::Mesh* pointerToStoredMesh = &engine->meshFactory.meshes[meshIndex];
        engine->meshFactory.meshNamePointerMap[mesh.name] = pointerToStoredMesh;

        engine->meshFactory.availableMeshNames.push_back(mesh.name);

    }

    Game::Mesh* GetMeshPointerByName(Engine* engine, const std::string& name) {
        if (!engine || name.empty()) {
            return nullptr;
        }

        auto it = engine->meshFactory.meshNamePointerMap.find(name);
        if (it != engine->meshFactory.meshNamePointerMap.end()) {
            // Found it, return the stored pointer
            return it->second;
        } else {
            // Name not found in the map
            // printf("Warning: GetMeshPointerByName - Mesh '%s' not found in map.\n", name.c_str());
            return nullptr;
        }
    }

    


    void InitMeshFactory(Engine* engine)
    {
        engine->meshFactory.meshes = MakeDynamicArray<Game::Mesh>(&engine->permanentMemory, 100);
    }

} // Zayn