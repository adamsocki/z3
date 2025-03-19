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


    void InitMyImgui(RenderManager* renderManager, WindowManager* windowManager) {
        VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                                              { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                                              { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                                              { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                                              { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                                              { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                                              { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                                              { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                                              { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                                              { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                                              { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;



        //VkDescriptorPool imguiPool;
        vkCreateDescriptorPool(renderManager->vulkanData.vkDevice, &pool_info, nullptr, &renderManager->myImgui.imGuiDescriptorPool);
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
            if (vkCreateRenderPass(renderManager->vulkanData.vkDevice, &info, nullptr, &renderManager->myImgui.imGuiRenderPass) !=
                VK_SUCCESS) {
                throw std::runtime_error("Could not create Dear ImGui's render pass");
            }
        }

        // 2: initialize imgui library

        // this initializes the core structures of imgui

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;

        // enable mouse cursor for ImGui
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        io.WantCaptureMouse = true;

        io.WantCaptureKeyboard = true;


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

    void UpdateMyImgui(RenderManager* renderManager, WindowManager* windowManager)
    {
        // Recording ImGui Command Buffer
        {
            // ImGui Render command
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();

            //some imgui UI to test
            ImGui::ShowDemoWindow();

//            ImGui::Begin("Debug Tools");
//            if (ImGui::CollapsingHeader("Render Stats"))
//            {
//                ImGui::Text("Objects: %d", zaynMem->gameObjects.size());
//                ImGui::Text("Materials: %d", zaynMem->materials.size());
//            }
//            ImGui::End();




            //LevelEditorIMGUI(zaynMem);






            ImGui::Render();

            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            VkResult err =
                    vkBeginCommandBuffer(renderManager->myImgui.imGuiCommandBuffers[renderManager->vulkanData.vkCurrentFrame], &info);
            //check_vk_result(err);

            // Render pass
            {
                VkClearValue clearValue = {};
                clearValue.color = { {0.0f, 0.0f, 0.0f, 1.0f} };
                // Copy passed clear color
                memcpy(&clearValue.color.float32[0], &clear_color, 4 * sizeof(float));

                VkRenderPassBeginInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                info.renderPass = renderManager->myImgui.imGuiRenderPass;
                info.framebuffer = renderManager->myImgui.imGuiFrameBuffers[renderManager->vulkanData.vkCurrentImageIndex];
                // swapChainFramebuffers[currentFrame];
                info.renderArea.extent.width = renderManager->vulkanData.vkSwapChainExtent.width;
                info.renderArea.extent.height = renderManager->vulkanData.vkSwapChainExtent.height;
                info.clearValueCount = 1;
                info.pClearValues = &clearValue;
                vkCmdBeginRenderPass(renderManager->myImgui.imGuiCommandBuffers[renderManager->vulkanData.vkCurrentFrame], &info,
                                     VK_SUBPASS_CONTENTS_INLINE);
            }

            // ImGui Render command
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), renderManager->myImgui.imGuiCommandBuffers[renderManager->vulkanData.vkCurrentFrame]);

            // Submit command buffer
            vkCmdEndRenderPass(renderManager->myImgui.imGuiCommandBuffers[renderManager->vulkanData.vkCurrentFrame]);
            {
                err = vkEndCommandBuffer(renderManager->myImgui.imGuiCommandBuffers[renderManager->vulkanData.vkCurrentFrame]);
                //check_vk_result(err);
            }
        }
    }
} // Zayn