//
// Created by socki on 3/4/2025.
//

#include "TextureManager.h"
#include "Engine.h"
#include "RenderManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"



namespace Zayn {



    std::string getTexturePath(const std::string& filename)
    {
        return "src/render/textures/" + filename;

    //  #ifdef WINDOWS
         //return "C:/dev_c/zayn/models/textures/" + filename;
    //#else
    //        return "/Users/socki/dev/zayn2/models/textures/" + filename;
    //#endif
    }


    void CreateTextureImage(Zayn::RenderManager* renderManager, uint32_t& mipLevels, VkImage* textureImage, VkDeviceMemory* textureImageMemory, const std::string texturePath, VkFormat format)
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        Zayn::CreateBuffer(renderManager, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        void* data;
        vkMapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory);

        stbi_image_free(pixels);

        Zayn::CreateImage(texWidth, texHeight, mipLevels, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *textureImage, *textureImageMemory, renderManager); // added

        // TransitionImageLayout(zaynMem->vkTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, zaynMem);
        Zayn::TransitionImageLayout(renderManager, *textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
        Zayn::CopyBufferToImage(renderManager, stagingBuffer, *textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

        vkDestroyBuffer(renderManager->vulkanData.vkDevice, stagingBuffer, nullptr);
        vkFreeMemory(renderManager->vulkanData.vkDevice, stagingBufferMemory, nullptr);
        Zayn::GenerateMipmaps(renderManager, *textureImage, format, texWidth, texHeight, mipLevels);
    }

    void CreateTexture(Zayn::RenderManager* renderManager, TextureCreateInfo* info, Texture* texture)
    {
        CreateTextureImage(renderManager, texture->mipLevels, &texture->image, &texture->memory, getTexturePath(info->path), info->format);
        //CreateTextureImageView(renderManager, texture->mipLevels, &texture->image, &texture->view);

//        CreateTextureSampler(zaynMem, texture->mipLevels, &texture->sampler);
    }

    void InitTextureManager(Engine* engine)
    {

    }

} // Zayn