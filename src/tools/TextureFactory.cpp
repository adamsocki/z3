//
// Created by socki on 3/5/2025.
//

#include <stdexcept>
#include <fstream>
#include "TextureFactory.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include "../Engine.h"


namespace Zayn {

    std::string GetTexturePath(const std::string& filename)
    {
        // First, try a relative path from the executable
        std::string relativePath = "../src/render/textures/" + filename;
        
        // If the file exists at the relative path, use that
        std::ifstream testFile(relativePath);
        if (testFile.good()) {
            testFile.close();
            return relativePath;
        }
        
        // If relative path doesn't work, try common absolute paths
        #ifdef WIN32
            // Try Windows-specific paths
            std::string winPath = "C:/dev/z3/src/render/textures/" + filename;
            testFile.open(winPath);
            if (testFile.good()) {
                testFile.close();
                return winPath;
            }
        #elif __APPLE__
            // Try Mac-specific paths
            std::string macPath = "/Users/socki/dev/z3/src/render/textures/" + filename;
            testFile.open(macPath);
            if (testFile.good()) {
                testFile.close();
                return macPath;
            }
        #endif
        
        // If all else fails, return the relative path as a fallback
        printf("Warning: Unable to find texture file: %s, falling back to relative path\n", filename.c_str());
        return relativePath;
    }

    void CreateTextureSampler(RenderManager* renderManager, uint32_t& mipLevels, VkSampler* textureSampler)
    {
        VkPhysicalDeviceProperties properties = {};
        vkGetPhysicalDeviceProperties(renderManager->vulkanData.vkPhysicalDevice, &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mipLevels);
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(renderManager->vulkanData.vkDevice, &samplerInfo, nullptr, textureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void CreateTextureImageView(RenderManager* renderManager, uint32_t& mipLevels, VkImage* textureImage, VkImageView* textureImageView)
    {
        *textureImageView = CreateImageView(*textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, renderManager);
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
    void MakeTexture(Engine* engine, TextureCreateInfo* info)
    {
        Game::Texture texture = {};
        RenderManager* renderManager = &engine->renderManager;
        texture.name = info->name;

        CreateTextureImage(renderManager, texture.mipLevels, &texture.image, &texture.memory, GetTexturePath(info->path), info->format);
        CreateTextureImageView(renderManager, texture.mipLevels, &texture.image, &texture.view);
        CreateTextureSampler(renderManager, texture.mipLevels, &texture.sampler);

        PushBack(&engine->textureFactory.textures, texture);
    }

    void InitTextureFactory(Engine* engine)
    {
        engine->textureFactory.textures = MakeDynamicArray<Game::Texture>(&engine->permanentMemory, 100);
    }

} // Zayn