

#include "RenderManager.h"
#include "MaterialManager.h"
#include "Engine.h"
#include "game/ModelPushConstant.h"
#include "game/UniformBufferObject.h"
#include "game/CameraManager.h"
#include "game/entities/PlayerEntity.h"
#include "MyImgui.h"
//#include "tools/TextureFactory.h"


#include <iostream>
#include <set>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <fstream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

//#include "include/stb_image.h"



#ifdef NDEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

/*
#ifdef WINDOWS
#elif __APPLE__
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset" };
#endif
*/





bool checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    VkLayerProperties* availableLayers = (VkLayerProperties*)malloc(layerCount * sizeof(VkLayerProperties));

    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for (int j = 0; j < layerCount; j++)
        {
            if (strcmp(layerName, availableLayers[j].layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            free(availableLayers);
            return false;
        }
    }
    free(availableLayers);
    return true;
}

std::vector<const char*> getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#ifdef __APPLE__
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
    }
#ifdef __APPLE__
    uint32_t instanceExtensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> availableInstanceExtensions(instanceExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableInstanceExtensions.data());
    for (const auto& extension : availableInstanceExtensions)
    {
        if (strcmp(extension.extensionName, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) == 0)
        {
            extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            break;
        }
    }
#endif
    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    // Try to extract more information about the error
    if (strstr(pCallbackData->pMessage, "renderPass is VK_NULL_HANDLE")) {
        std::cerr << "CUSTOM DEBUG: Detected renderPass NULL handle error!" << std::endl;
        // Print call stack or other debug info if possible
    }
    return VK_FALSE;
}


void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
}

void PrintGPUName(VkInstance instance)
{
    // Enumerate physical devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Print the name of each physical device
    for (const auto& device : devices)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        std::cout << "GPU: " << deviceProperties.deviceName << std::endl;
    }
}



void CreateVKInstance(Zayn::RenderManager* renderManager)
{
    if (enableValidationLayers && !checkValidationLayerSupport())
    {
        std::cerr << "Validation layers requested, but not available!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Zayn";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Zayn Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &renderManager->vulkanData.vkInstance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
    PrintGPUName(renderManager->vulkanData.vkInstance);
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}


void SetupDebugMessenger(Zayn::RenderManager* renderManager)
{
    if (!enableValidationLayers)
    {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    // populateDebugMessengerCreateInfo(createInfo);
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional


    VkAllocationCallbacks pAllocator = {};


#ifdef __APPLE__
    if (CreateDebugUtilsMessengerEXT(Zayn->vkInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), &pAllocator, &Zayn->vkDebugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
#else
    if (CreateDebugUtilsMessengerEXT(renderManager->vulkanData.vkInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, &renderManager->vulkanData.vkDebugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
#endif

}

void CreateSurface(Zayn::RenderManager* renderManager, Zayn::WindowManager* window)
{
    if (glfwCreateWindowSurface(renderManager->vulkanData.vkInstance, window->glfwWindow, nullptr, &renderManager->vulkanData.vkSurface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

Zayn::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, Zayn::RenderManager* renderManager)
{
    Zayn::SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, renderManager->vulkanData.vkSurface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, renderManager->vulkanData.vkSurface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, renderManager->vulkanData.vkSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, renderManager->vulkanData.vkSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, renderManager->vulkanData.vkSurface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

Zayn::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, Zayn::RenderManager* renderManager)
{
    Zayn::QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i{};
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, renderManager->vulkanData.vkSurface, &presentSupport);

        if (presentSupport)
        {
            indices.presentFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}


bool isDeviceSuitable(VkPhysicalDevice device, Zayn::RenderManager* renderManager)
{
    Zayn::QueueFamilyIndices indices = findQueueFamilies(device, renderManager);
    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        Zayn::SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, renderManager);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    //VkPhysicalDeviceFeatures supportedFeatures;
    //vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    //return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

void PickPhysicalDevice(Zayn::RenderManager* renderManager)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(renderManager->vulkanData.vkInstance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(renderManager->vulkanData.vkInstance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        // printDeviceExtensions(device);
        if (isDeviceSuitable(device, renderManager))
        {
            renderManager->vulkanData.vkPhysicalDevice = device;
            break;
        }
    }

    if (renderManager->vulkanData.vkPhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void CreateLogicalDevice(Zayn::RenderManager* renderManager)
{
    Zayn::QueueFamilyIndices indices = findQueueFamilies(renderManager->vulkanData.vkPhysicalDevice, renderManager);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(renderManager->vulkanData.vkPhysicalDevice, &createInfo, nullptr, &renderManager->vulkanData.vkDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(renderManager->vulkanData.vkDevice, indices.graphicsFamily.value(), 0, &renderManager->vulkanData.vkGraphicsQueue);
    vkGetDeviceQueue(renderManager->vulkanData.vkDevice, indices.presentFamily.value(), 0, &renderManager->vulkanData.vkPresentQueue);
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    std::cout << "Present mode: V-Sync?" << std::endl;

    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
        // UNCOMMENT BELOW IF YOU WANT TO NOT HAVE VSYNC ON
        /* else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
         {
             std::cout << "Present mode: Immediate (No VSync)" << std::endl;
             return availablePresentMode;
         }*/
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, Zayn::WindowManager* window)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window->glfwWindow, &width, &height);

        VkExtent2D actualExtent =
                {
                        static_cast<uint32_t>(width),
                        static_cast<uint32_t>(height)
                };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}



void CreateSwapChain(Zayn::RenderManager* renderManager, Zayn::WindowManager* window)
{
    Zayn::SwapChainSupportDetails swapChainSupport = querySwapChainSupport(renderManager->vulkanData.vkPhysicalDevice, renderManager);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = renderManager->vulkanData.vkSurface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    Zayn::QueueFamilyIndices indices = findQueueFamilies(renderManager->vulkanData.vkPhysicalDevice, renderManager);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(renderManager->vulkanData.vkDevice, &createInfo, nullptr, &renderManager->vulkanData.vkSwapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(renderManager->vulkanData.vkDevice, renderManager->vulkanData.vkSwapChain, &imageCount, nullptr);
    renderManager->vulkanData.vkSwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(renderManager->vulkanData.vkDevice, renderManager->vulkanData.vkSwapChain, &imageCount, renderManager->vulkanData.vkSwapChainImages.data());

    renderManager->vulkanData.vkSwapChainImageFormat = surfaceFormat.format;
    renderManager->vulkanData.vkSwapChainExtent = extent;
}

void CreateImageViews(Zayn::RenderManager* renderManager)
{
    renderManager->vulkanData.vkSwapChainImageViews.resize(renderManager->vulkanData.vkSwapChainImages.size());

    for (size_t i = 0; i < renderManager->vulkanData.vkSwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = renderManager->vulkanData.vkSwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = renderManager->vulkanData.vkSwapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(renderManager->vulkanData.vkDevice, &createInfo, nullptr, &renderManager->vulkanData.vkSwapChainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, Zayn::RenderManager* renderManager)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(renderManager->vulkanData.vkPhysicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}



VkFormat FindDepthFormat(Zayn::RenderManager* renderManager)
{
    return FindSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, renderManager);
}

void CreateRenderPass(Zayn::RenderManager* renderManager)
{

    std::cout << "Starting render pass creation..." << std::endl;

    // Check if device is valid
    if (renderManager->vulkanData.vkDevice == VK_NULL_HANDLE) {
        std::cerr << "ERROR: vkDevice is NULL when creating render pass!" << std::endl;
        return;
    }

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = renderManager->vulkanData.vkSwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;


    // changed for imgui
#ifdef IMGUI
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
#else
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
#endif



    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = FindDepthFormat(renderManager);
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(renderManager->vulkanData.vkDevice, &renderPassInfo, nullptr, &renderManager->vulkanData.vkRenderPass) != VK_SUCCESS)
    {
        std::cerr << "ERROR: Failed to create render pass!" << std::endl;
        throw std::runtime_error("failed to create render pass!");
    }
    else {
        std::cout << "Render pass created successfully. Handle: " << renderManager->vulkanData.vkRenderPass << std::endl;
    }

#if IMGUI


    // Attachment
    colorAttachment = {};
    colorAttachment.format = renderManager->vulkanData.vkSwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // No MSAA
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Color VkAttachmentReference our render pass needs.
    colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // subpass
    subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // synchronization and dependency
    dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0; // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &colorAttachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;
    if (vkCreateRenderPass(renderManager->vulkanData.vkDevice, &info, nullptr, &renderManager->myImgui.imGuiRenderPass) !=
        VK_SUCCESS) {
        throw std::runtime_error("Could not create Dear ImGui's render pass");
    }

#endif
}

void CreateCommandPool(Zayn::RenderManager* renderManager)
{
    Zayn::QueueFamilyIndices queueFamilyIndices = findQueueFamilies(renderManager->vulkanData.vkPhysicalDevice, renderManager);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    //poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(renderManager->vulkanData.vkDevice, &poolInfo, nullptr, &renderManager->vulkanData.vkCommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }

#if IMGUI

    if (vkCreateCommandPool(renderManager->vulkanData.vkDevice, &poolInfo, nullptr, &renderManager->myImgui.imGuiCommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }

#endif
}

uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, Zayn::RenderManager* renderManager)
{
    // VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(renderManager->vulkanData.vkPhysicalDevice, &renderManager->vulkanData.vkMemProperties);
    for (uint32_t i = 0; i < renderManager->vulkanData.vkMemProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (renderManager->vulkanData.vkMemProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

void Zayn::CreateImage(uint32_t width,
                 uint32_t height,
                 uint32_t mipLevels,
                 VkFormat format,
                 VkImageTiling tiling,
                 VkImageUsageFlags usage,
                 VkMemoryPropertyFlags properties,
                 VkImage& image,
                 VkDeviceMemory& imageMemory,
                 Zayn::RenderManager* renderManager)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(renderManager->vulkanData.vkDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(renderManager->vulkanData.vkDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties, renderManager);

    if (vkAllocateMemory(renderManager->vulkanData.vkDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(renderManager->vulkanData.vkDevice, image, imageMemory, 0);
}


VkCommandBuffer BeginSingleTimeCommands(Zayn::RenderManager* renderManager)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = renderManager->vulkanData.vkCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(renderManager->vulkanData.vkDevice, &allocInfo, &commandBuffer);

#if IMGUI
    renderManager->myImgui.imGuiCommandBuffers.resize(
    renderManager->vulkanData.vkSwapChainImageViews.size());

    allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = renderManager->myImgui.imGuiCommandPool;
    allocInfo.commandBufferCount =
    static_cast<uint32_t>(renderManager->myImgui.imGuiCommandBuffers.size());
    vkAllocateCommandBuffers(renderManager->vulkanData.vkDevice, &allocInfo, renderManager->myImgui.imGuiCommandBuffers.data());

#endif

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void EndSingleTimeCommands(Zayn::RenderManager* renderManager, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(renderManager->vulkanData.vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(renderManager->vulkanData.vkGraphicsQueue);

    vkFreeCommandBuffers(renderManager->vulkanData.vkDevice, renderManager->vulkanData.vkCommandPool, 1, &commandBuffer);
}

void Zayn::TransitionImageLayout(Zayn::RenderManager* renderManager, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(renderManager);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        //destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = (renderManager->vulkanData.vkGraphicsQueue) ? VK_PIPELINE_STAGE_TRANSFER_BIT : VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

    EndSingleTimeCommands(renderManager, commandBuffer);
}

void Zayn::GenerateMipmaps(Zayn::RenderManager* renderManager, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(renderManager->vulkanData.vkPhysicalDevice, imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(renderManager);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
                       image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &blit,
                       VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);

    EndSingleTimeCommands(renderManager, commandBuffer);
}

void Zayn::CopyBufferToImage(Zayn::RenderManager* renderManager, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(renderManager);

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
            width,
            height,
            1 };

    vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);

    EndSingleTimeCommands(renderManager, commandBuffer);
}



VkImageView Zayn::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, Zayn::RenderManager* renderManager)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(renderManager->vulkanData.vkDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}


void CreateDepthResources(Zayn::RenderManager* renderManager)
{
    VkFormat depthFormat = FindDepthFormat(renderManager);

    CreateImage(renderManager->vulkanData.vkSwapChainExtent.width, renderManager->vulkanData.vkSwapChainExtent.height, 1, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderManager->vulkanData.vkDepthImage, renderManager->vulkanData.vkDepthImageMemory, renderManager);
    renderManager->vulkanData.vkDepthImageView = CreateImageView(renderManager->vulkanData.vkDepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, renderManager);
}


void CreateFrameBuffers(Zayn::RenderManager* renderManager)
{
    if (!renderManager) {
        std::cerr << "ERROR: renderManager is NULL!" << std::endl;
        return;
    }

    std::cout << "Starting framebuffer creation..." << std::endl;
    std::cout << "renderManager address: " << renderManager << std::endl;
    std::cout << "vkRenderPass handle in CreateFrameBuffers: " << renderManager->vulkanData.vkRenderPass << std::endl;

    std::cout << "Framebuffers vector size before resize: "
              << renderManager->vulkanData.vkSwapChainFramebuffers.size() << std::endl;

    // Check if render pass is valid
    if (renderManager->vulkanData.vkRenderPass == VK_NULL_HANDLE) {
        std::cerr << "ERROR: vkRenderPass is NULL when creating framebuffers!" << std::endl;
        return;
    }
    renderManager->vulkanData.vkSwapChainFramebuffers.resize(renderManager->vulkanData.vkSwapChainImageViews.size());
    std::cout << "Resized framebuffers vector to: "
              << renderManager->vulkanData.vkSwapChainFramebuffers.size() << std::endl;


#if IMGUI

    renderManager->myImgui.imGuiFrameBuffers.resize(renderManager->vulkanData.vkSwapChainImageViews.size());

#endif

    for (size_t i = 0; i < renderManager->vulkanData.vkSwapChainImageViews.size(); i++)
    {
        std::cout << "Creating framebuffer " << i << "..." << std::endl;

        std::array<VkImageView, 2> attachments = {
                renderManager->vulkanData.vkSwapChainImageViews[i],
                renderManager->vulkanData.vkDepthImageView
        };

        std::cout << "Attachments: " << attachments[0] << ", " << attachments[1] << std::endl;

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderManager->vulkanData.vkRenderPass;

        std::cout << "Using vkRenderPass in framebufferInfo: " << framebufferInfo.renderPass << std::endl;

        // Print the rest of framebufferInfo for debugging
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = renderManager->vulkanData.vkSwapChainExtent.width;
        framebufferInfo.height = renderManager->vulkanData.vkSwapChainExtent.height;
        framebufferInfo.layers = 1;

        std::cout << "Before vkCreateFramebuffer call..." << std::endl;
        VkResult result = vkCreateFramebuffer(
                renderManager->vulkanData.vkDevice,
                &framebufferInfo,
                nullptr,
                &renderManager->vulkanData.vkSwapChainFramebuffers[i]
        );
        std::cout << "vkCreateFramebuffer result: " << result << std::endl;

        if (result != VK_SUCCESS) {
            std::cerr << "ERROR creating framebuffer " << i << ": " << result << std::endl;
            throw std::runtime_error("failed to create framebuffer!");
        }

        std::cout << "Framebuffer " << i << " created successfully." << std::endl;


#if IMGUI

        // Imgui framebuffer
        VkImageView imgui_attachment[1];
        framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderManager->myImgui.imGuiRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = imgui_attachment;
        framebufferInfo.width = renderManager->vulkanData.vkSwapChainExtent.width;
        framebufferInfo.height = renderManager->vulkanData.vkSwapChainExtent.height;
        framebufferInfo.layers = 1;
        imgui_attachment[0] = renderManager->vulkanData.vkSwapChainImageViews[i];
        if (vkCreateFramebuffer(renderManager->vulkanData.vkDevice, &framebufferInfo, nullptr,
            &renderManager->myImgui.imGuiFrameBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create ImGui framebuffer!");
        }

#endif
    }
}


void StartRender_Init(Zayn::RenderManager* renderManager, Zayn::WindowManager* window)
{
    CreateVKInstance(renderManager);
    SetupDebugMessenger(renderManager);
    CreateSurface(renderManager, window);
    PickPhysicalDevice(renderManager);    // <-- @TODO: May need to modify to account for other machines like rPi
    CreateLogicalDevice(renderManager);   // is SAME

    CreateSwapChain(renderManager, window);
    CreateImageViews(renderManager);

    CreateRenderPass(renderManager);

    CreateCommandPool(renderManager);
    CreateDepthResources(renderManager);
    CreateFrameBuffers(renderManager);
}





void Zayn::CopyBuffer(Zayn::RenderManager* renderManager, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(renderManager);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    EndSingleTimeCommands(renderManager, commandBuffer);
}




void Zayn::CreateBuffer(Zayn::RenderManager* renderManager, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage | VK_BUFFER_CREATE_SPARSE_BINDING_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(renderManager->vulkanData.vkDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(renderManager->vulkanData.vkDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties, renderManager);

    if (vkAllocateMemory(renderManager->vulkanData.vkDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(renderManager->vulkanData.vkDevice, buffer, bufferMemory, 0);
}


void CreateDescriptorSetLayout(Zayn::RenderManager* renderManager, VkDescriptorSetLayout* descriptorSetLayout, bool hasImage)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings = {};
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings.push_back(uboLayoutBinding);

    if (hasImage)
    {
        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings.push_back(samplerLayoutBinding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(renderManager->vulkanData.vkDevice, &layoutInfo, nullptr, descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void CreateDescriptorPool(Zayn::RenderManager* renderManager, VkDescriptorPool* descriptorPool, bool hasImage)
{
    std::vector<VkDescriptorPoolSize> poolSizes = {};
    VkDescriptorPoolSize poolSize_1{};
    poolSize_1.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize_1.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 20);
    poolSizes.push_back(poolSize_1);
    if (hasImage)
    {
        VkDescriptorPoolSize poolSize_2 = {};
        poolSize_2.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize_2.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 20);
        poolSizes.push_back(poolSize_2);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 20);

    if (vkCreateDescriptorPool(renderManager->vulkanData.vkDevice, &poolInfo, nullptr, descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

VkShaderModule CreateShaderModule(Zayn::RenderManager* renderManager, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(renderManager->vulkanData.vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

static std::vector<char> ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}


template <typename T>
void CreatePushConstant(Zayn::RenderManager* renderManager)
{
    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(T);

    renderManager->vulkanData.vkPushConstantRanges.push_back(pushConstantRange);
}

void  CreateGraphicsPipeline(Zayn::RenderManager* renderManager, VkPipeline* pipeline, const std::string& vertShaderFilePath, const std::string& fragShaderFilePath, std::vector<VkPushConstantRange> pushConstants, VkDescriptorSetLayout* descriptorSetLayout, VkPipelineLayout* pipelineLayout)
{
    auto vertShaderCode = ReadFile(vertShaderFilePath);
    auto fragShaderCode = ReadFile(fragShaderFilePath);

    VkShaderModule vertShaderModule = CreateShaderModule(renderManager, vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(renderManager, fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Zayn::Vertex::getBindingDescription();
    auto attributeDescriptions = Zayn::Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional

    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {};  // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayout;

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(Game::ModelPushConstant);
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(renderManager->vulkanData.vkDevice, &pipelineLayoutInfo, nullptr, pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = *pipelineLayout;
    pipelineInfo.renderPass = renderManager->vulkanData.vkRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(renderManager->vulkanData.vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(renderManager->vulkanData.vkDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(renderManager->vulkanData.vkDevice, vertShaderModule, nullptr);
}

std::string GetShaderPath(const std::string& filename) {
//#ifdef WINDOWS
    return "../src/render/shaders/" + filename;
    //return "/src/renderer/shaders/" + filename;
//#else
    //return "/Users/socki/dev/zayn2/src/renderer/shaders/" + filename;
//#endif
}

void CreateUniformBuffer(Zayn::RenderManager* renderManager, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void*>& uniformBuffersMapped)
{
    VkDeviceSize bufferSize = sizeof(Game::UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        Zayn::CreateBuffer(renderManager, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
        vkMapMemory(renderManager->vulkanData.vkDevice, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }
}

void CreateCommandBuffers(Zayn::RenderManager* renderManager)
{
    // Zayn->vkCommandBuffers = (VkCommandBuffer *)malloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT);
    renderManager->vulkanData.vkCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = renderManager->vulkanData.vkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    if (vkAllocateCommandBuffers(renderManager->vulkanData.vkDevice, &allocInfo, renderManager->vulkanData.vkCommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
#if IMGUI
    // ImGui Command Buffer
    //zaynMem->imGuiCommandBuffers.resize(zaynMem->vkSwapChainImageViews.size()); // or MAX_FRAMES_IN_FLIGHT?
    renderManager->myImgui.imGuiCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    // create command buffers
    allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = renderManager->myImgui.imGuiCommandPool;
    allocInfo.commandBufferCount =
        static_cast<uint32_t>(renderManager->myImgui.imGuiCommandBuffers.size());
    vkAllocateCommandBuffers(renderManager->vulkanData.vkDevice, &allocInfo, renderManager->myImgui.imGuiCommandBuffers.data());
#endif
}

void CreateSyncObjects(Zayn::RenderManager* renderManager)
{
    renderManager->vulkanData.vkImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderManager->vulkanData.vkRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderManager->vulkanData.vkInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    renderManager->vulkanData.vkImagesInFlight.resize(renderManager->vulkanData.vkSwapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(renderManager->vulkanData.vkDevice, &semaphoreInfo, nullptr, &renderManager->vulkanData.vkImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(renderManager->vulkanData.vkDevice, &semaphoreInfo, nullptr, &renderManager->vulkanData.vkRenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(renderManager->vulkanData.vkDevice, &fenceInfo, nullptr, &renderManager->vulkanData.vkInFlightFences[i]) != VK_SUCCESS)
        {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}


void Zayn::InitRenderManager(Zayn::RenderManager* renderManager, Zayn::WindowManager* window)
{
    std::cout << "InitRender_Vulkan()" << std::endl;
    std::cout << "InitRender_Vulkan() with renderManager address: " << renderManager << std::endl;

    // I DONT THINK ANY GAME SPECIFIC THINGS OCCUR HERE

    StartRender_Init(renderManager, window);

#if IMGUI
    InitMyImgui(renderManager, window);


#endif
    std::cout << "after InitRender_Vulkan()" << std::endl;

// INIT MATERIAL SYSTEM

    //zaynMem->materialSystem = new MaterialSystem(&zaynMem->vulkan.vkDevice);

    CreateDescriptorSetLayout(renderManager, &renderManager->vulkanData.vkDescriptorSetLayout, true); // this one is for those that have texures attached
    CreateDescriptorPool(renderManager, &renderManager->vulkanData.vkDescriptorPool, true);      // <---- CAN POTENTIAL BE RESUSED BETWEEN ENTITIES THAT HAVE THE SAME TYPES OF THINGS BEING SHARED

    CreatePushConstant<Game::ModelPushConstant>(renderManager);

    CreateGraphicsPipeline(renderManager, &renderManager->vulkanData.vkGraphicsPipeline, GetShaderPath("vkShader_3d_INIT_vert.spv"), GetShaderPath("vkShader_3d_INIT_frag.spv"), renderManager->vulkanData.vkPushConstantRanges, &renderManager->vulkanData.vkDescriptorSetLayout, &renderManager->vulkanData.vkPipelineLayout);

    CreateUniformBuffer(renderManager, renderManager->vulkanData.vkUniformBuffers, renderManager->vulkanData.vkUniformBuffersMemory, renderManager->vulkanData.vkUniformBuffersMapped);

    CreateCommandBuffers(renderManager);
    CreateSyncObjects(renderManager);

    std::cout << "after InitRender_Vulkan() with vkRenderPass: " << renderManager->vulkanData.vkRenderPass << std::endl;
}

VkResult AcquireNextImage(Zayn::RenderManager* renderManager, uint32_t* imageIndex)
{
    vkWaitForFences(renderManager->vulkanData.vkDevice, 1, &renderManager->vulkanData.vkInFlightFences[renderManager->vulkanData.vkCurrentFrame], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(
            renderManager->vulkanData.vkDevice,
            renderManager->vulkanData.vkSwapChain,
            UINT64_MAX,
            renderManager->vulkanData.vkImageAvailableSemaphores[renderManager->vulkanData.vkCurrentFrame], // must be a not signaled semaphore
            VK_NULL_HANDLE,
            imageIndex);

    return result;
}

void CleanUpSwapChain(Zayn::RenderManager* renderManager)
{
    vkDestroyImageView(renderManager->vulkanData.vkDevice, renderManager->vulkanData.vkDepthImageView, nullptr);
    vkDestroyImage(renderManager->vulkanData.vkDevice, renderManager->vulkanData.vkDepthImage, nullptr);
    vkFreeMemory(renderManager->vulkanData.vkDevice, renderManager->vulkanData.vkDepthImageMemory, nullptr);

    for (size_t i = 0; i < renderManager->vulkanData.vkSwapChainFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(renderManager->vulkanData.vkDevice, renderManager->vulkanData.vkSwapChainFramebuffers[i], nullptr);
    }

    for (size_t i = 0; i < renderManager->vulkanData.vkSwapChainImageViews.size(); i++)
    {
        vkDestroyImageView(renderManager->vulkanData.vkDevice, renderManager->vulkanData.vkSwapChainImageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(renderManager->vulkanData.vkDevice, renderManager->vulkanData.vkSwapChain, nullptr);
}

void RecreateSwapChain(Zayn::RenderManager* renderManager, Zayn::WindowManager* windowManager)
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(windowManager->glfwWindow, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(windowManager->glfwWindow, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(renderManager->vulkanData.vkDevice);

    CleanUpSwapChain(renderManager);
    CreateSwapChain(renderManager, windowManager);
    CreateImageViews(renderManager);
    CreateDepthResources(renderManager);
    CreateFrameBuffers(renderManager);
}

bool BeginFrameRender(Zayn::RenderManager* renderManager, Zayn::WindowManager* windowManager)
{
    assert(!renderManager->vulkanData.vkIsFrameStarted && "cannot call begin frame when frame buffer is already in progress");
    auto result = AcquireNextImage(renderManager, &renderManager->vulkanData.vkCurrentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain(renderManager, windowManager);
        return false;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    renderManager->vulkanData.vkIsFrameStarted = true;

    // vkResetFences(Zayn->vkDevice, 1, &Zayn->vkInFlightFences[Zayn->vkCurrentFrame]);
    // vkResetCommandBuffer(Zayn->vkCommandBuffers[Zayn->vkCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(renderManager->vulkanData.vkCommandBuffers[renderManager->vulkanData.vkCurrentFrame], &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
        return false;
    }

    return true;
}


void UpdateUniformBuffer(uint32_t currentImage, Zayn::RenderManager* renderManager, Game::CameraManager* cam)
{
    // static auto startTime = std::chrono::high_resolution_clock::now();

    // auto currentTime = std::chrono::high_resolution_clock::now();
    // float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


//    Zayn::CameraManager* cam = cam;

    Game::UniformBufferObject ubo = {};
    // ubo.model = TRS(V3(0.0f, 0.0f, 0.0f), AxisAngle(V3(0.0f, 0.2f, 0.20f), time * DegToRad(10.0f)), V3(1.0f, 1.0f, 1.0f));

    // apply view based on camera rotation

    cam->front.x = cosf(DegToRad(cam->yaw)) * cosf(DegToRad(cam->pitch));
    cam->front.y = sinf(DegToRad(cam->pitch));
    cam->front.z = sinf(DegToRad(cam->yaw)) * cosf(DegToRad(cam->pitch));
    cam->front = Normalize(cam->front);

    glm::vec3 camPos = glm::vec3(cam->pos.x, cam->pos.y, cam->pos.z);
    glm::vec3 camFront = glm::vec3(cam->front.x, cam->front.y, cam->front.z);
    glm::vec3 camUp = glm::vec3(cam->up.x, cam->up.y, cam->up.z);

    ubo.view = glm::lookAt(camPos, camPos + camFront, camUp);
    // ubo.view = glm::lookAt(cam->pos, cam->pos + cam->front, cam->up);

    ubo.proj = glm::perspective(glm::radians(60.0f), renderManager->vulkanData.vkSwapChainExtent.width / (float)renderManager->vulkanData.vkSwapChainExtent.height, 0.1f, 1000.0f);
    ubo.proj[1][1] *= -1;


    memcpy(renderManager->vulkanData.vkUniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void BeginSwapChainRenderPass(Zayn::RenderManager* renderManager, VkCommandBuffer commandBuffer)
{
    assert(renderManager->vulkanData.vkIsFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == renderManager->vulkanData.vkCommandBuffers[renderManager->vulkanData.vkCurrentFrame] && "Can't begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderManager->vulkanData.vkRenderPass;
    renderPassInfo.framebuffer = renderManager->vulkanData.vkSwapChainFramebuffers[renderManager->vulkanData.vkCurrentImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = renderManager->vulkanData.vkSwapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(renderManager->vulkanData.vkSwapChainExtent.width);
    viewport.height = static_cast<float>(renderManager->vulkanData.vkSwapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{ {0, 0}, renderManager->vulkanData.vkSwapChainExtent };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void RenderEntities(Zayn::Engine* engine, VkCommandBuffer commandBuffer)
{
    //uint32_t dynamicOffset = zaynMem->vulkan.vkCurrentFrame * sizeof(UniformBufferObject);
    Game::PlayerEntity* playerEntity = (Game::PlayerEntity*)Zayn::GetEntity(&engine->entityFactory, engine->HTEST);
    for (int i = 0; i < 1; i++)
    {
       // GameObject& gameObj = zaynMem->gameObjects[i];
        VkDescriptorSet& set = playerEntity->material->descriptorSets[engine->renderManager.vulkanData.vkCurrentFrame];

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, engine->renderManager.vulkanData.vkGraphicsPipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, engine->renderManager.vulkanData.vkPipelineLayout, 0, 1, &set, 0, nullptr);


        // Push constants for the transform
        /* ModelPushConstant pushConstant = {};
         pushConstant.model_1 = TRS((V3(0.0f, 1.0f, -1.0f)), AxisAngle(V3(0.0f, 0.2f, 0.20f), 0.0f), V3(1.0f, 1.0f, 1.0f));*/

        vkCmdPushConstants(commandBuffer, engine->renderManager.vulkanData.vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Game::ModelPushConstant), &playerEntity->pushConstantData);

        // Bind the vertex and index buffers
        VkBuffer vertexBuffers[] = { playerEntity->mesh->vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffer, playerEntity->mesh->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        // Draw the mesh
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(playerEntity->mesh->indices.size()), 1, 0, 0, 0);

    }


}

void EndSwapChainRenderPass(Zayn::RenderManager* renderManager, VkCommandBuffer commandBuffer)
{
    assert(renderManager->vulkanData.vkIsFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == renderManager->vulkanData.vkCommandBuffers[renderManager->vulkanData.vkCurrentFrame] && "Can't begin render pass on command buffer from a different frame");

    vkCmdEndRenderPass(commandBuffer);
}

VkResult SubmitCommandBuffers(Zayn::RenderManager* renderManager, std::vector<VkCommandBuffer> buffers, uint32_t* imageIndex)
{

    if (renderManager->vulkanData.vkImagesInFlight[*imageIndex] != VK_NULL_HANDLE)
    {
        vkWaitForFences(renderManager->vulkanData.vkDevice, 1, &renderManager->vulkanData.vkImagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
    }
    renderManager->vulkanData.vkImagesInFlight[*imageIndex] = renderManager->vulkanData.vkImagesInFlight[renderManager->vulkanData.vkCurrentFrame];
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { renderManager->vulkanData.vkImageAvailableSemaphores[renderManager->vulkanData.vkCurrentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = static_cast<uint32_t>(buffers.size());
    // submitInfo.commandBufferCount
    submitInfo.pCommandBuffers = buffers.data();

    VkSemaphore signalSemaphores[] = { renderManager->vulkanData.vkRenderFinishedSemaphores[renderManager->vulkanData.vkCurrentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(renderManager->vulkanData.vkDevice, 1, &renderManager->vulkanData.vkInFlightFences[renderManager->vulkanData.vkCurrentFrame]);
    if (vkQueueSubmit(renderManager->vulkanData.vkGraphicsQueue, 1, &submitInfo, renderManager->vulkanData.vkInFlightFences[renderManager->vulkanData.vkCurrentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { renderManager->vulkanData.vkSwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = imageIndex;
    // presentInfo.pImageIndices = &Zayn->vkCurrentImageIndex;

    auto result = vkQueuePresentKHR(renderManager->vulkanData.vkPresentQueue, &presentInfo);

    renderManager->vulkanData.vkCurrentFrame = (renderManager->vulkanData.vkCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;

}

void EndFrameRender(Zayn::RenderManager* renderManager, Zayn::WindowManager* windowManager)
{
    assert(renderManager->vulkanData.vkIsFrameStarted && "Can't call endFrame while frame is not in progress");

    std::vector<VkCommandBuffer> submitCommandBuffers = {};

    submitCommandBuffers.push_back(renderManager->vulkanData.vkCommandBuffers[renderManager->vulkanData.vkCurrentFrame]);

#if IMGUI

    submitCommandBuffers.push_back(renderManager->myImgui.imGuiCommandBuffers[renderManager->vulkanData.vkCurrentFrame]);
    std::cout << "hitIMGUI" <<std::endl;
#endif

    if (vkEndCommandBuffer(submitCommandBuffers[0]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }

    auto result = SubmitCommandBuffers(renderManager, submitCommandBuffers, &renderManager->vulkanData.vkCurrentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || renderManager->vulkanData.vkFramebufferResized)
    {
        renderManager->vulkanData.vkFramebufferResized = false;
        RecreateSwapChain(renderManager, windowManager);
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }
    renderManager->vulkanData.vkIsFrameStarted = false;
}

void Zayn::UpdateRenderManager(Zayn::Engine* engine, Zayn::EntityHandle handle, Zayn::RenderManager* renderManager, Zayn::WindowManager* windowManager, Game::CameraManager* cameraManager)
{


    if (BeginFrameRender(renderManager, windowManager))
    {
#if IMGUI
        UpdateMyImgui(renderManager, windowManager);


#endif
        UpdateUniformBuffer(renderManager->vulkanData.vkCurrentFrame, renderManager, cameraManager);

        BeginSwapChainRenderPass(renderManager, renderManager->vulkanData.vkCommandBuffers[renderManager->vulkanData.vkCurrentFrame]);

        RenderEntities(engine, renderManager->vulkanData.vkCommandBuffers[renderManager->vulkanData.vkCurrentFrame]);

    }

    EndSwapChainRenderPass(renderManager, renderManager->vulkanData.vkCommandBuffers[renderManager->vulkanData.vkCurrentFrame]);

    EndFrameRender(renderManager, windowManager);

}