//
// Created by socki on 3/18/2025.
//

#include <vulkan/vulkan_core.h>
#include "MyImgui.h"
#include "RenderManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace Zayn {


    void ToggleImGuiVisibility(RenderManager *renderManager) {
        // Toggle the visibility flag
        renderManager->myImgui.visible = !renderManager->myImgui.visible;

        // Optional: Print debug message
        printf("ImGui visibility: %s\n", renderManager->myImgui.visible ? "ON" : "OFF");
    }


    void InitMyImgui(RenderManager *renderManager, WindowManager *windowManager) {
        VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
                                             {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                             {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
                                             {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
                                             {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
                                             {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
                                             {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
                                             {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
                                             {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                             {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                             {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000}};

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = (uint32_t) std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;



        //VkDescriptorPool imguiPool;
        vkCreateDescriptorPool(renderManager->vulkanData.vkDevice, &pool_info, nullptr,
                               &renderManager->myImgui.imGuiDescriptorPool);
        {
            // Attachment
            VkAttachmentDescription colorAttachment = {};
            colorAttachment.format = renderManager->vulkanData.vkSwapChainImageFormat;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // No MSAA
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            // Color VkAttachmentReference our render pass needs.
            VkAttachmentReference colorAttachmentRef = {};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            // subpass
            VkSubpassDescription subpass = {};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;

            // synchronization and dependency
            VkSubpassDependency dependency = {};
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
            if (vkCreateRenderPass(renderManager->vulkanData.vkDevice, &info, nullptr,
                                   &renderManager->myImgui.imGuiRenderPass) !=
                VK_SUCCESS) {
                throw std::runtime_error("Could not create Dear ImGui's render pass");
            }
        }

        // 2: initialize imgui library

        // this initializes the core structures of imgui

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;

        // enable mouse cursor for ImGui
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

//        io.WantCaptureMouse = true;
//        io.WantCaptureKeyboard = true;


        // this initializes imgui for SDL

        ImGui_ImplGlfw_InitForVulkan(windowManager->glfwWindow, true);

        // this initializes imgui for Vulkan
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = renderManager->vulkanData.vkInstance;
        init_info.PhysicalDevice = renderManager->vulkanData.vkPhysicalDevice;
        init_info.Device = renderManager->vulkanData.vkDevice;
        init_info.Queue = renderManager->vulkanData.vkGraphicsQueue;
        init_info.DescriptorPool = renderManager->myImgui.imGuiDescriptorPool;

        init_info.RenderPass = renderManager->myImgui.imGuiRenderPass;
        {
            // Already sized to the minImageCount + 1 &&  < MaxImageCount
            std::uint32_t imageCount = static_cast<uint32_t>(renderManager->vulkanData.vkSwapChainImages.size());
            init_info.MinImageCount = imageCount;
            init_info.ImageCount = imageCount;
        }

        ImGui_ImplVulkan_Init(&init_info);

        ImGui_ImplVulkan_CreateFontsTexture();
    }

    void UpdateMyImgui(Game::CameraManager *cameraManager, RenderManager *renderManager, WindowManager *windowManager, InputManager *inputManager)
    {
        // Always set up the ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Only render UI elements if ImGui is visible
        if (renderManager->myImgui.visible) {
            // Demo window
            ImGui::ShowDemoWindow();

            // IMGUI INTERFACE CODE
            ImGui::Begin("Debug Tools");

            // SECTION 1
            ImGui::Text("Press F1 to toggle ImGui visibility");
            if (ImGui::Button("Hide ImGui")) {
                ToggleImGuiVisibility(renderManager);
            }

            if (ImGui::CollapsingHeader("Mouse Information", ImGuiTreeNodeFlags_DefaultOpen)) {
                // Get mouse position from InputManager
                vec2 mousePos = GetMousePosition(inputManager);
                vec2 delta = GetMouseDelta(inputManager);
                vec2 normPos = GetMousePositionNormalized(inputManager);
                vec2 normSignedPos = GetMousePositionNormalizedSigned(inputManager);

                ImGui::Text("Cursor Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
                ImGui::Text("Cursor Delta: (%.1f, %.1f)", delta.x, delta.y);

                ImGui::Text("Raw Delta X: %.1f", inputManager->mouse->analogue[Input_MousePositionXOffset]);
                ImGui::Text("Raw Delta Y: %.1f", inputManager->mouse->analogue[Input_MousePositionYOffset]);

                // Display normalized coordinates
                ImGui::Text("Normalized Position [0,1]: (%.3f, %.3f)", normPos.x, normPos.y);
                ImGui::Text("Normalized Position [-1,1]: (%.3f, %.3f)", normSignedPos.x, normSignedPos.y);

                // Add visualization of mouse position
                ImVec2 windowPos = ImGui::GetWindowPos();
                ImVec2 windowSize = ImGui::GetWindowSize();
                float visualizationSize = 100.0f;

                ImGui::Separator();
                ImGui::Text("Position Visualization:");

                ImDrawList *drawList = ImGui::GetWindowDrawList();
                ImVec2 vizPos = ImVec2(windowPos.x + 20, ImGui::GetCursorScreenPos().y);

                // Draw border
                drawList->AddRect(vizPos,
                                  ImVec2(vizPos.x + visualizationSize, vizPos.y + visualizationSize),
                                  IM_COL32(255, 255, 255, 255));

                // Draw cursor position
                ImVec2 cursorDot = ImVec2(
                        vizPos.x + normPos.x * visualizationSize,
                        vizPos.y + normPos.y * visualizationSize
                );

                drawList->AddCircleFilled(cursorDot, 4.0f, IM_COL32(255, 0, 0, 255));

                ImGui::Dummy(ImVec2(visualizationSize + 20, visualizationSize + 10));
            }

            if (ImGui::CollapsingHeader("Render Stats")) {
                ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
            }

            ImGui::End();
        }

        // Always render (even if nothing was drawn)
        ImGui::Render();

        // Always record command buffer and process render pass
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VkResult err = vkBeginCommandBuffer(
                renderManager->myImgui.imGuiCommandBuffers[renderManager->vulkanData.vkCurrentFrame],
                &info);

        // Render pass always processes, even for empty ImGui
        VkClearValue clearValue = {};
        clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        memcpy(&clearValue.color.float32[0], &clear_color, 4 * sizeof(float));

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderManager->myImgui.imGuiRenderPass;
        renderPassInfo.framebuffer = renderManager->myImgui.imGuiFrameBuffers[renderManager->vulkanData.vkCurrentImageIndex];
        renderPassInfo.renderArea.extent.width = renderManager->vulkanData.vkSwapChainExtent.width;
        renderPassInfo.renderArea.extent.height = renderManager->vulkanData.vkSwapChainExtent.height;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearValue;

        vkCmdBeginRenderPass(
                renderManager->myImgui.imGuiCommandBuffers[renderManager->vulkanData.vkCurrentFrame],
                &renderPassInfo,
                VK_SUBPASS_CONTENTS_INLINE);

        // Always render ImGui draw data
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                        renderManager->myImgui.imGuiCommandBuffers[renderManager->vulkanData.vkCurrentFrame]);

        // Always end the render pass and command buffer
        vkCmdEndRenderPass(
                renderManager->myImgui.imGuiCommandBuffers[renderManager->vulkanData.vkCurrentFrame]);

        err = vkEndCommandBuffer(
                renderManager->myImgui.imGuiCommandBuffers[renderManager->vulkanData.vkCurrentFrame]);
    }
} // Zayn

