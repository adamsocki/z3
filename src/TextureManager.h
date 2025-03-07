////
//// Created by socki on 3/4/2025.
////
//
//#ifndef Z3_TEXTUREMANAGER_H
//#define Z3_TEXTUREMANAGER_H
//
//#include <string>
//#include <vulkan/vulkan_core.h>
//#include "tools/TextureFactory.h"
//
//
//namespace Zayn {
//
//    struct Engine;
//
//    struct Texture {
//        std::string name;
//        VkImage image;
//        VkDeviceMemory memory;
//        VkImageView view;
//        VkSampler sampler;
//        uint32_t width;
//        uint32_t height;
//        uint32_t mipLevels = 1;
//
//        bool isInitialized = false;
//    };
//
//    struct TextureManager {
//        TextureFactory factory;
//    };
//
//    //void CreateTexture();
//    std::string GetTexturePath(const std::string& filename);
//    void InitTextureManager(Engine* engine);
//
//} // Zayn
//
//#endif //Z3_TEXTUREMANAGER_H
