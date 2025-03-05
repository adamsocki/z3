//
// Created by socki on 3/4/2025.
//

#ifndef Z3_TEXTUREMANAGER_H
#define Z3_TEXTUREMANAGER_H

#include <string>
#include <vulkan/vulkan_core.h>


namespace Zayn {

    struct Engine;
    struct TextureCreateInfo
{
        std::string path;
        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
        bool generateMipmaps;
        //VkFilter filter;
        VkSamplerAddressMode addressMode;
};

    struct Texture {
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

    struct TextureManager {

    };

    void CreateTexture();
    void InitTextureManager(Engine* engine);

} // Zayn

#endif //Z3_TEXTUREMANAGER_H
