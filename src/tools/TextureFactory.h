//
// Created by socki on 3/5/2025.
//

#ifndef Z3_TEXTUREFACTORY_H
#define Z3_TEXTUREFACTORY_H

#include <vulkan/vulkan_core.h>
#include "../game/Texture.h"
#include "../RenderManager.h"
//#include "../Engine.h"
#include "../DynamicArray.h"

namespace Zayn {

//    struct Engine;
    struct TextureCreateInfo
    {
        std::string path;
        std::string name;
        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
        bool generateMipmaps;
        //VkFilter filter;
        VkSamplerAddressMode addressMode;
    };


    struct TextureFactory {
        DynamicArray<Game::Texture> textures;
    };

    void MakeTexture(Engine* engine, TextureCreateInfo* info);

    void InitTextureFactory(Engine* engine);
} // Zayn

#endif //Z3_TEXTUREFACTORY_H
