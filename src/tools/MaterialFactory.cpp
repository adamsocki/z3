//
// Created by socki on 3/5/2025.
//

#include "MaterialFactory.h"
#include "../Engine.h"
#include "../game/UniformBufferObject.h"


namespace Zayn {



    bool AllocateMaterialDescriptorSet(Engine* engine, Game::Material* material, uint32_t frameIndex) {
        RenderManager* renderManager = &engine->renderManager;
        // Validate required components
        if (!material->texture || !material->texture->view || !material->texture->sampler) {
            fprintf(stderr, "Material missing valid albedo texture\n");
            return false;
        }

        VkDescriptorSetLayout layout = material->texture
                                       ? renderManager->vulkanData.vkDescriptorSetLayout
                                       : renderManager->vulkanData.vkDescriptorSetLayout_blank;

        VkDescriptorPool pool = material->texture
                                ? renderManager->vulkanData.vkDescriptorPool
                                : renderManager->vulkanData.vkDescriptorPool_blank;
        // Descriptor set allocation
        VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };

        allocInfo.descriptorPool = renderManager->vulkanData.vkDescriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &renderManager->vulkanData.vkDescriptorSetLayout;

        VkResult result = vkAllocateDescriptorSets(renderManager->vulkanData.vkDevice, &allocInfo, &material->descriptorSets[frameIndex]);
        if (result != VK_SUCCESS) {
            fprintf(stderr, "Failed to allocate material descriptor set: %d\n", result);
            return false;
        }

        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = renderManager->vulkanData.vkUniformBuffers[frameIndex]; // Use frame's uniform buffer
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        // Bind the material's texture to the descriptor set
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = material->texture->sampler;
        imageInfo.imageView = material->texture->view;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = material->descriptorSets[frameIndex];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = material->descriptorSets[frameIndex];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(renderManager->vulkanData.vkDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);


        return true;
    }

    void MakeMaterial(Engine* engine, MaterialCreateInfo* info)
    {


        Game::Material material = {};
        material.type = info->type;
        //outMaterial->color = info->color;
        //memcpy(outMaterial->color, info->color, sizeof(float) * 4);
        material.roughness = info->roughness;
        material.metallic = info->metallic;
        material.texture = info->texture;
        material.name = info->name;

        material.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            AllocateMaterialDescriptorSet(engine, &material, i);
        }

        PushBack(&engine->materialFactory.materials, material);
    }

    void InitMaterialFactory(Engine* engine)
    {
        DynamicArray<Game::Material> materials = MakeDynamicArray<Game::Material>(&engine->permanentMemory, 100);
    }



} // Zayn