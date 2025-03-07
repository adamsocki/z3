


#include "RenderManager.h"
#include "MaterialManager.h"
//#include "tools/TextureFactory.h"


#include <iostream>
#include <set>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <algorithm>

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
        throw std::runtime_error("failed to create render pass!");
    }
    else {
        std::cout << "Render pass created successfully." << std::endl;
    }

#if IMGUI


//    // Attachment
//    colorAttachment = {};
//    colorAttachment.format = renderManager->vulkanData.vkSwapChainImageFormat;
//    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // No MSAA
//    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
//    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//
//    // Color VkAttachmentReference our render pass needs.
//    colorAttachmentRef = {};
//    colorAttachmentRef.attachment = 0;
//    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//    // subpass
//    subpass = {};
//    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//    subpass.colorAttachmentCount = 1;
//    subpass.pColorAttachments = &colorAttachmentRef;
//
//    // synchronization and dependency
//    dependency = {};
//    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//    dependency.dstSubpass = 0;
//    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//    dependency.srcAccessMask = 0; // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//
//    VkRenderPassCreateInfo info = {};
//    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//    info.attachmentCount = 1;
//    info.pAttachments = &colorAttachment;
//    info.subpassCount = 1;
//    info.pSubpasses = &subpass;
//    info.dependencyCount = 1;
//    info.pDependencies = &dependency;
//    if (vkCreateRenderPass(renderManager->vulkanData.vkDevice, &info, nullptr, &Zayn->myIMGUI.imGuiRenderPass) !=
//        VK_SUCCESS) {
//        throw std::runtime_error("Could not create Dear ImGui's render pass");
//    }

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

   /* if (vkCreateCommandPool(renderManager->vulkanData..vkDevice, &poolInfo, nullptr, &zaynMem->myIMGUI.imGuiCommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }*/

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
//    Zayn->myIMGUI.imGuiCommandBuffers.resize(
//    renderManager->vulkanData.vkSwapChainImageViews.size());
//
//    allocInfo = {};
//    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//    allocInfo.commandPool = Zayn->myIMGUI.imGuiCommandPool;
//    allocInfo.commandBufferCount =
//    static_cast<uint32_t>(Zayn->myIMGUI.imGuiCommandBuffers.size());
//    vkAllocateCommandBuffers(renderManager->vulkanData.vkDevice, &allocInfo, Zayn->myIMGUI.imGuiCommandBuffers.data());

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
    renderManager->vulkanData.vkSwapChainFramebuffers.resize(renderManager->vulkanData.vkSwapChainImageViews.size());

#if IMGUI

   // Zayn->myIMGUI.imGuiFrameBuffers.resize(Zayn->vulkan.vkSwapChainImageViews.size());

#endif

    for (size_t i = 0; i < renderManager->vulkanData.vkSwapChainImageViews.size(); i++)
    {
        std::array<VkImageView, 2> attachments = {
                renderManager->vulkanData.vkSwapChainImageViews[i],
                renderManager->vulkanData.vkDepthImageView };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderManager->vulkanData.vkRenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width  = renderManager->vulkanData.vkSwapChainExtent.width;
        framebufferInfo.height = renderManager->vulkanData.vkSwapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(renderManager->vulkanData.vkDevice, &framebufferInfo, nullptr, &renderManager->vulkanData.vkSwapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }

#if IMGUI

        // Imgui framebuffer
//        VkImageView imgui_attachment[1];
//        framebufferInfo = {};
//        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//        framebufferInfo.renderPass = Zayn->myIMGUI.imGuiRenderPass;
//        framebufferInfo.attachmentCount = 1;
//        framebufferInfo.pAttachments = imgui_attachment;
//        framebufferInfo.width = Zayn->vulkan.vkSwapChainExtent.width;
//        framebufferInfo.height = Zayn->vulkan.vkSwapChainExtent.height;
//        framebufferInfo.layers = 1;
//        imgui_attachment[0] = Zayn->vulkan.vkSwapChainImageViews[i];
//        if (vkCreateFramebuffer(Zayn->vulkan.vkDevice, &framebufferInfo, nullptr,
//            &Zayn->myIMGUI.imGuiFrameBuffers[i]) != VK_SUCCESS)
//        {
//            throw std::runtime_error("failed to create ImGui framebuffer!");
//        }

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




void Zayn::InitRenderManager(Zayn::RenderManager* renderManager, Zayn::WindowManager* window)
{
    std::cout << "InitRender_Vulkan()" << std::endl;
    // I DONT THINK ANY GAME SPECIFIC THINGS OCCUR HERE
    StartRender_Init(renderManager, window);
    std::cout << "after InitRender_Vulkan()" << std::endl;

// INIT MATERIAL SYSTEM

    //zaynMem->materialSystem = new MaterialSystem(&zaynMem->vulkan.vkDevice);
    
}

/*



void Zayn::InitRenderer(RenderManager* renderManager)
{
    std::cout << "InitRender_Vulkan()" << std::endl;
    // I DONT THINK ANY GAME SPECIFIC THINGS OCCUR HERE
    StartRender_Init(zaynMem);
    std::cout << "after InitRender_Vulkan()" << std::endl;



    // INIT MATERIAL SYSTEM
    zaynMem->materialSystem = new MaterialSystem(&zaynMem->vulkan.vkDevice);



    // CUSTOM CODE FOR RENDERS @TODO need to make this simpler
    CreateDescriptorSetLayout(&zaynMem->vulkan.vkDescriptorSetLayout, true, zaynMem); // this one is for those that have texures attached
    CreateDescriptorPool(zaynMem, &zaynMem->vulkan.vkDescriptorPool, true);      // <---- CAN POTENTIAL BE RESUSED BETWEEN ENTITIES THAT HAVE THE SAME TYPES OF THINGS BEING SHARED
    

    CreatePushConstant<ModelPushConstant>(zaynMem);
    
    CreateGraphicsPipeline(zaynMem, &zaynMem->vulkan.vkGraphicsPipeline, getShaderPath("vkShader_3d_INIT_vert.spv"), getShaderPath("vkShader_3d_INIT_frag.spv"), zaynMem->vulkan.vkPushConstantRanges, &zaynMem->vulkan.vkDescriptorSetLayout, &zaynMem->vulkan.vkPipelineLayout);
    


    // for new system  1. 
    //CreateTextureImage(zaynMem, zaynMem->texture_001.mipLevels, &zaynMem->texture_001.image, &zaynMem->texture_001.memory, getTexturePath("viking_room.png"), VK_FORMAT_R8G8B8A8_SRGB);
    //CreateTextureImageView(zaynMem, zaynMem->texture_001.mipLevels, &zaynMem->texture_001.image, &zaynMem->texture_001.view);
    //CreateTextureSampler(zaynMem, zaynMem->texture_001.mipLevels, &zaynMem->texture_001.sampler); 
    ////CreateMesh(zaynMem, getModelPath("viking_room.obj"), zaynMem->gameObject.mesh);
    //LoadModel(getModelPath("viking_room.obj"), &zaynMem->mesh_001.vertices, &zaynMem->mesh_001.indices);
    //CreateVertexBuffer(zaynMem, zaynMem->mesh_001.vertices, &zaynMem->mesh_001.vertexBuffer, &zaynMem->mesh_001.vertexBufferMemory);
    //CreateIndexBuffer(zaynMem, zaynMem->mesh_001.indices, &zaynMem->mesh_001.indexBuffer, &zaynMem->mesh_001.indexBufferMemory);



    //CreateGameObject(zaynMem);

   
   


   /* CreateTextureImage(zaynMem, zaynMem->vulkan.vkMipLevels, &zaynMem->vulkan.vkTextureImage, &zaynMem->vulkan.vkTextureImageMemory, getTexturePath("viking_room.png"), VK_FORMAT_R8G8B8A8_SRGB);
    CreateTextureImageView(zaynMem, zaynMem->vulkan.vkMipLevels, &zaynMem->vulkan.vkTextureImage, &zaynMem->vulkan.vkTextureImageView);
    CreateTextureSampler(zaynMem, zaynMem->vulkan.vkMipLevels, &zaynMem->vulkan.vkTextureSampler);     
    LoadModel(getModelPath("viking_room.obj"), &zaynMem->vulkan.vkVertices, &zaynMem->vulkan.vkIndices);
    CreateVertexBuffer(zaynMem, zaynMem->vulkan.vkVertices, &zaynMem->vulkan.vkVertexBuffer, &zaynMem->vulkan.vkVertexBufferMemory);
    CreateIndexBuffer(zaynMem, zaynMem->vulkan.vkIndices, &zaynMem->vulkan.vkIndexBuffer, &zaynMem->vulkan.vkIndexBufferMemory);
    #1#
    CreateUniformBuffer(zaynMem, zaynMem->vulkan.vkUniformBuffers, zaynMem->vulkan.vkUniformBuffersMemory, zaynMem->vulkan.vkUniformBuffersMapped);
    
    
    //CreateDescriptorSets(zaynMem, true, sizeof(UniformBufferObject), zaynMem->vulkan.vkUniformBuffers, &zaynMem->vulkan.vkDescriptorSetLayout, &zaynMem->vulkan.vkDescriptorPool, zaynMem->vulkan.vkDescriptorSets, &zaynMem->vulkan.vkTextureImageView, &zaynMem->vulkan.vkTextureSampler);


    
    EndRender_Init(zaynMem);
}
*/
