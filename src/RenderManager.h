//
// Created by socki on 3/3/2025.
//

#ifndef Z2_RENDERMANAGER_H
#define Z2_RENDERMANAGER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "data_types.h"
#include "math/math.h"
#include <optional>
#include <glm/glm.hpp>
#include <array>
#include <cstring>

#include <set>
#include "WindowManager.h"


namespace Zayn {

    struct Engine;


    struct InstancedData
    {
        mat4 modelMatrix;
    };

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;
        glm::vec3 normal;

        mat4 modelMatrix;

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputBindingDescription, 2>  getBindingDescriptions_instanced()
        {
            std::array<VkVertexInputBindingDescription, 2> bindingDescriptions{};
            bindingDescriptions[0].binding = 0;
            bindingDescriptions[0].stride = sizeof(Vertex);
            bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            bindingDescriptions[1].binding = 1;
            bindingDescriptions[1].stride = sizeof(InstancedData);
            bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

            return bindingDescriptions;
        }

        static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(Vertex, normal);

            return attributeDescriptions;
        }

        static std::array<VkVertexInputAttributeDescription, 8> getAttributeDescriptions_instanced()
        {
            std::array<VkVertexInputAttributeDescription, 8> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(Vertex, normal);

            attributeDescriptions[4].binding = 1; // Assuming binding 1 for instance data
            attributeDescriptions[4].location = 4;
            attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[4].offset = offsetof(InstancedData, modelMatrix) + sizeof(glm::vec4) * 0;

            attributeDescriptions[5].binding = 1; // Assuming binding 1 for instance data
            attributeDescriptions[5].location = 5;
            attributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[5].offset = offsetof(InstancedData, modelMatrix) + sizeof(glm::vec4) * 1;

            attributeDescriptions[6].binding = 1; // Assuming binding 1 for instance data
            attributeDescriptions[6].location = 6;
            attributeDescriptions[6].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[6].offset = offsetof(InstancedData, modelMatrix) + sizeof(glm::vec4) * 2;

            attributeDescriptions[7].binding = 1; // Assuming binding 1 for instance data
            attributeDescriptions[7].location = 7;
            attributeDescriptions[7].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[7].offset = offsetof(InstancedData, modelMatrix) + sizeof(glm::vec4) * 3;

            return attributeDescriptions;
        }

        bool operator==(const Vertex& other) const {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };


    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct VulkanData {
        bool vkFramebufferResized;
        VkInstance vkInstance;
        VkDebugUtilsMessengerEXT vkDebugMessenger;
        VkSurfaceKHR vkSurface;

        VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
        VkDevice vkDevice;
        VkQueue vkGraphicsQueue;

        VkQueue vkPresentQueue;


        VkSwapchainKHR vkSwapChain;

        std::vector<VkImage> vkSwapChainImages;
        std::vector<VkImageView> vkSwapChainImageViews;

        VkExtent2D vkSwapChainExtent;
        VkFormat vkSwapChainImageFormat;
        VkRenderPass vkRenderPass;

        VkCommandPool vkCommandPool;

        VkPhysicalDeviceMemoryProperties vkMemProperties;

        std::vector<VkImage> vkDepthImages;
        std::vector<VkDeviceMemory> vkDepthImageMemorys;
        std::vector<VkImageView> vkDepthImageViews;

        std::vector<VkFramebuffer> vkSwapChainFramebuffers;

        std::vector<VkCommandBuffer> vkCommandBuffers;

        std::vector<VkSemaphore> vkImageAvailableSemaphores;
        std::vector<VkSemaphore> vkRenderFinishedSemaphores;
        std::vector<VkFence> vkInFlightFences;
        std::vector<VkFence> vkImagesInFlight;

        VkImage vkDepthImage;
        VkDeviceMemory vkDepthImageMemory;
        VkImageView vkDepthImageView;

        uint32_t vkQueueFamilyCount = 0;  // TODO: FIX THIS




        // CUSTOM CODE FOR RENDERS @TODO need to make this simpler
        VkDescriptorSetLayout vkDescriptorSetLayout;
        VkDescriptorSetLayout vkDescriptorSetLayout_blank;
        std::vector<VkPushConstantRange> vkPushConstantRanges;
        VkPipeline vkGraphicsPipeline;
        VkPipeline vkGraphicsPipeline_blank;
        VkPipelineLayout vkPipelineLayout;
        VkShaderModule vkVertShaderModule;
        VkShaderModule vkFragShaderModule;
        uint32_t vkMipLevels;
        VkImage vkTextureImage;
        VkDeviceMemory vkTextureImageMemory;
        VkImageView vkTextureImageView;
        VkSampler vkTextureSampler;
        std::vector<Vertex> vkVertices;
        std::vector<uint32_t> vkIndices;
        VkBuffer vkVertexBuffer;
        VkDeviceMemory vkVertexBufferMemory;
        VkBuffer vkIndexBuffer;
        VkDeviceMemory vkIndexBufferMemory;
        std::vector<VkBuffer> vkUniformBuffers;
        std::vector<VkDeviceMemory> vkUniformBuffersMemory;
        std::vector<void*> vkUniformBuffersMapped;
        VkDescriptorPool vkDescriptorPool;
        VkDescriptorPool vkDescriptorPool_blank;
        std::vector<VkDescriptorSet> vkDescriptorSets;
        uint32_t vkCurrentFrame = 0;
        uint32 vkCurrentImageIndex;
        bool vkIsFrameStarted = false;
    };

    struct RenderManager {

        VulkanData vulkanData;
    };

    void InitRenderManager(RenderManager* renderManager, WindowManager* window);

} // Zayn

#endif //Z2_RENDERMANAGER_H
