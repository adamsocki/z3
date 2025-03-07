//
// Created by socki on 3/5/2025.
//

#ifndef Z3_TEXTURE_H
#define Z3_TEXTURE_H

#include <string>
#include <vulkan/vulkan_core.h>

namespace Game {

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

} // Game

#endif //Z3_TEXTURE_H
