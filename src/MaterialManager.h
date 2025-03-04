//
// Created by socki on 3/4/2025.
//

#ifndef Z2_MATERIALMANAGER_H
#define Z2_MATERIALMANAGER_H

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace Zayn {


    struct Texture
    {
        std::string name;
        VkImage image;
        VkDeviceMemory memory;
        VkImageView view;
        VkSampler sampler;
        uint32_t width;
        uint32_t height;
        uint32_t mipLevels = 1;

        bool isInitialized = false;
    };

    enum MaterialType
    {
        MATERIAL_PBR,
        MATERIAL_UNLIT
    };

    struct Material
    {
        std::string name;
        MaterialType type;
        std::vector<VkDescriptorSet> descriptorSets;
        Texture* texture;
        float color[4];
        float metallic;
        float roughness;
        bool isInitialized = false;
    };




    struct MaterialManager {



    };

} // Zayn

#endif //Z2_MATERIALMANAGER_H
