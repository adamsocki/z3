//
// Created by socki on 3/2/2025.
//


#include "Engine.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "GameManager.h"
#include "tools/MaterialFactory.h"
#include "tools/MeshFactory.h"
#include "PhysicsManager.h"


namespace Zayn {

    void InitEngine(Engine* engine)
    {
        engine->inLevelEditor = true;
        InitMemoryManager(engine);
        InitWindowManager(engine, V2(1500,1033), "spanglish");
        InitTimeManager(engine);
        InitInputManager(engine);



        // InitPhysicsManager();

        InitComponentFactory(&engine->componentFactory, &engine->permanentMemory);
        // InitPhysicsFactory(&engine->physicsFactory, &engine->permanentMemory);

        InitTextureFactory(engine);
        InitMaterialFactory(engine);
        InitMeshFactory(engine);


        InitEntityFactory(&engine->entityFactory);

//        InitGameObjectFactory(engine);
//        InitMaterialManager(engine);

        InitCameraManager( &engine->cameraManager, engine->windowManager.glfwWindow, &engine->inputManager);


        InitRenderManager(&engine->renderManager, &engine->windowManager);
        InitPhysicsManager(engine, &engine->renderManager);

        InitLevelEditor(&engine->levelEditor, engine);


//        InitCameraManager(engine);

        InitGameManager(engine);
    }

    void UpdateEngine(Engine* engine)
    {
        // Start OF FRAME
        UpdateTimeManager(engine);
        UpdateInputManager(engine);
        UpdateCameraManager(&engine->windowManager, &engine->cameraManager, &engine->inputManager, &engine->timeManager);

        UpdatePhysicsManager(engine);

        // START OF GAME Frame
        UpdateGameManager(engine);
        
        // Update physics system
        // UpdatePhysicsFactory(engine, engine->timeManager.time.deltaTime);
        // UpdatePhysicsManager(engine);
        
        // Update rendering
        UpdateRenderManager(engine, engine->HTEST, &engine->renderManager, &engine->windowManager, &engine->cameraManager, &engine->inputManager);


        // End OF FRAME
        ClearInputManager(engine);

    }

    void ShutdownEngine(Engine* engine)
    {
        // Wait for the device to finish all operations
        vkDeviceWaitIdle(engine->renderManager.vulkanData.vkDevice);

        // Clean up Vulkan resources in reverse order of creation
        
        // Clean up ImGui resources
        if (engine->renderManager.myImgui.imGuiDescriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(engine->renderManager.vulkanData.vkDevice, engine->renderManager.myImgui.imGuiDescriptorPool, nullptr);
        }
        
        for (auto framebuffer : engine->renderManager.myImgui.imGuiFrameBuffers) {
            vkDestroyFramebuffer(engine->renderManager.vulkanData.vkDevice, framebuffer, nullptr);
        }
        
        if (engine->renderManager.myImgui.imGuiRenderPass != VK_NULL_HANDLE) {
            vkDestroyRenderPass(engine->renderManager.vulkanData.vkDevice, engine->renderManager.myImgui.imGuiRenderPass, nullptr);
        }
        
        if (engine->renderManager.myImgui.imGuiCommandPool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(engine->renderManager.vulkanData.vkDevice, engine->renderManager.myImgui.imGuiCommandPool, nullptr);
        }
        
        // Clean up Vulkan resources for textures
        if (engine->renderManager.vulkanData.vkTextureSampler != VK_NULL_HANDLE) {
            vkDestroySampler(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkTextureSampler, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkTextureImageView != VK_NULL_HANDLE) {
            vkDestroyImageView(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkTextureImageView, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkTextureImage != VK_NULL_HANDLE) {
            vkDestroyImage(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkTextureImage, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkTextureImageMemory != VK_NULL_HANDLE) {
            vkFreeMemory(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkTextureImageMemory, nullptr);
        }
        
        // Clean up descriptor resources
        if (engine->renderManager.vulkanData.vkDescriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkDescriptorPool, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkDescriptorPool_blank != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkDescriptorPool_blank, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkDescriptorSetLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkDescriptorSetLayout, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkDescriptorSetLayout_blank != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkDescriptorSetLayout_blank, nullptr);
        }
        
        // Clean up uniform buffers
        for (size_t i = 0; i < engine->renderManager.vulkanData.vkUniformBuffers.size(); i++) {
            vkDestroyBuffer(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkUniformBuffers[i], nullptr);
            vkFreeMemory(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkUniformBuffersMemory[i], nullptr);
        }
        
        // Clean up vertex and index buffers
        if (engine->renderManager.vulkanData.vkIndexBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkIndexBuffer, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkIndexBufferMemory != VK_NULL_HANDLE) {
            vkFreeMemory(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkIndexBufferMemory, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkVertexBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkVertexBuffer, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkVertexBufferMemory != VK_NULL_HANDLE) {
            vkFreeMemory(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkVertexBufferMemory, nullptr);
        }
        
        // Clean up pipeline resources
        if (engine->renderManager.vulkanData.vkFragShaderModule != VK_NULL_HANDLE) {
            vkDestroyShaderModule(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkFragShaderModule, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkVertShaderModule != VK_NULL_HANDLE) {
            vkDestroyShaderModule(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkVertShaderModule, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkPipelineLayout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkPipelineLayout, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkGraphicsPipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkGraphicsPipeline, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkGraphicsPipeline_blank != VK_NULL_HANDLE) {
            vkDestroyPipeline(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkGraphicsPipeline_blank, nullptr);
        }
        
        // Clean up depth resources
        if (engine->renderManager.vulkanData.vkDepthImageView != VK_NULL_HANDLE) {
            vkDestroyImageView(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkDepthImageView, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkDepthImage != VK_NULL_HANDLE) {
            vkDestroyImage(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkDepthImage, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkDepthImageMemory != VK_NULL_HANDLE) {
            vkFreeMemory(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkDepthImageMemory, nullptr);
        }
        
        // Clean up depth image arrays
        for (size_t i = 0; i < engine->renderManager.vulkanData.vkDepthImageViews.size(); i++) {
            vkDestroyImageView(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkDepthImageViews[i], nullptr);
        }
        
        for (size_t i = 0; i < engine->renderManager.vulkanData.vkDepthImages.size(); i++) {
            vkDestroyImage(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkDepthImages[i], nullptr);
        }
        
        for (size_t i = 0; i < engine->renderManager.vulkanData.vkDepthImageMemorys.size(); i++) {
            vkFreeMemory(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkDepthImageMemorys[i], nullptr);
        }
        
        // Clean up synchronization objects
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (i < engine->renderManager.vulkanData.vkImageAvailableSemaphores.size())
                vkDestroySemaphore(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkImageAvailableSemaphores[i], nullptr);
            
            if (i < engine->renderManager.vulkanData.vkRenderFinishedSemaphores.size())
                vkDestroySemaphore(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkRenderFinishedSemaphores[i], nullptr);
            
            if (i < engine->renderManager.vulkanData.vkInFlightFences.size())
                vkDestroyFence(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkInFlightFences[i], nullptr);
        }
        
        // Clean up command pool
        if (engine->renderManager.vulkanData.vkCommandPool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkCommandPool, nullptr);
        }
        
        // Clean up framebuffers and swapchain resources
        for (auto framebuffer : engine->renderManager.vulkanData.vkSwapChainFramebuffers) {
            vkDestroyFramebuffer(engine->renderManager.vulkanData.vkDevice, framebuffer, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkRenderPass != VK_NULL_HANDLE) {
            vkDestroyRenderPass(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkRenderPass, nullptr);
        }
        
        for (auto imageView : engine->renderManager.vulkanData.vkSwapChainImageViews) {
            vkDestroyImageView(engine->renderManager.vulkanData.vkDevice, imageView, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkSwapChain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(engine->renderManager.vulkanData.vkDevice, engine->renderManager.vulkanData.vkSwapChain, nullptr);
        }
        
        // Clean up device, surface, and instance
        if (engine->renderManager.vulkanData.vkDevice != VK_NULL_HANDLE) {
            vkDestroyDevice(engine->renderManager.vulkanData.vkDevice, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkSurface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(engine->renderManager.vulkanData.vkInstance, engine->renderManager.vulkanData.vkSurface, nullptr);
        }
        
        if (engine->renderManager.vulkanData.vkDebugMessenger != VK_NULL_HANDLE) {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(engine->renderManager.vulkanData.vkInstance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr) {
                func(engine->renderManager.vulkanData.vkInstance, engine->renderManager.vulkanData.vkDebugMessenger, nullptr);
            }
        }
        
        if (engine->renderManager.vulkanData.vkInstance != VK_NULL_HANDLE) {
            vkDestroyInstance(engine->renderManager.vulkanData.vkInstance, nullptr);
        }
        
        // Clean up memory managers
        // Note: assuming memory arenas need cleanup
        // Free dynamic arrays if allocated
        // if (engine->levelEditor.currentLevelData.levelEntityHandles != nullptr) {
            // free(engine->levelEditor.currentLevelData.levelEntityHandles.data);
        // }

        DynamicArrayClear(&engine->levelEditor.currentLevelData.levelEntityHandles);

        printf("Engine shutdown complete\n");
    }
} // Zayn