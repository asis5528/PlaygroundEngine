#include "VulkanImgui.h"
#include <stdexcept>



VulkanImgui::VulkanImgui(VulkanBase* base, GLFWwindow* window) {
	this->base = base;
    this->window = window;
	ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
	IMGUI_CHECKVERSION();
    createImguiRenderPass();
    createFramebuffers();
    createCommandBuffers();
    createDescriptorPool();
   
}

VulkanImgui::~VulkanImgui()
{
    for (VkFramebuffer& fbuffer : imguiFramebuffers) {
        vkDestroyFramebuffer(base->vulkandevice->device, fbuffer, nullptr);
    }
   
    vkDestroyRenderPass(base->vulkandevice->device, imGuiRenderPass, nullptr);
    vkDestroyDescriptorPool(base->vulkandevice->device,imguiDescriptorPool, nullptr);
    ImGui_ImplVulkan_Shutdown();
}

void VulkanImgui::init() {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();  (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = base->vkinstance;
    init_info.PhysicalDevice = base->vkPhysicalDevice;
    init_info.Device = base->device;
    init_info.QueueFamily = base->vulkandevice->queueID;
    init_info.Queue = base->queue;
    init_info.PipelineCache = pipelineCache;
    init_info.DescriptorPool = imguiDescriptorPool;
    init_info.Allocator = allocator;
    init_info.MinImageCount = 2;
    init_info.ImageCount = base->swapChain->swapChainImages.size();
    ImGui_ImplVulkan_Init(&init_info, imGuiRenderPass);
    VkCommandBuffer command_buffer = base->vulkandevice->beginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
    base->vulkandevice->endSingleTimeCommands(command_buffer);

   
}
void VulkanImgui::draw(uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(imguiCommandBuffer[imageIndex], &beginInfo);
    {
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = imGuiRenderPass;
        info.framebuffer = imguiFramebuffers[imageIndex];
        info.renderArea.extent.width = base->swapChain->swapChainExtent.width;
        info.renderArea.extent.height = base->swapChain->swapChainExtent.height;
        info.clearValueCount = 1;
        std::array<VkClearValue, 1> clearValues{};
        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        //learValues[1].depthStencil = { 1.0f, 0 };

        info.pClearValues = clearValues.data();
        vkCmdBeginRenderPass(imguiCommandBuffer[imageIndex], &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), imguiCommandBuffer[imageIndex]);
    }
    {
        vkCmdEndRenderPass(imguiCommandBuffer[imageIndex]);
        vkEndCommandBuffer(imguiCommandBuffer[imageIndex]);

    }
}
void VulkanImgui::recreate() {
    createFramebuffers();
}
void VulkanImgui::createImguiRenderPass()
{
    VkAttachmentDescription attachment = {};
    attachment.format = base->swapChain->swapChainImageFormat;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment = {};
    color_attachment.attachment = 0;
    color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &attachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;
    if (vkCreateRenderPass(base->device, &info, nullptr, &imGuiRenderPass) != VK_SUCCESS) {
        throw std::runtime_error("Could not create Dear ImGui's render pass");
    }
}

void VulkanImgui::createFramebuffers()
{
    imguiFramebuffers.resize(base->swapChain->swapChainImageViews.size());

    for (size_t i = 0; i < base->swapChain->swapChainImageViews.size(); i++) {
       
        
        {
            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = base->swapChain->renderPass;
            framebufferInfo.width = base->swapChain->swapChainExtent.width;
            framebufferInfo.height = base->swapChain->swapChainExtent.height;
            framebufferInfo.layers = 1;
            std::array<VkImageView, 1>attachments = { base->swapChain->swapChainImageViews[i] };
            framebufferInfo.renderPass = imGuiRenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            if (vkCreateFramebuffer(base->device, &framebufferInfo, nullptr, &imguiFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }

        }

    }

}

void VulkanImgui::createCommandBuffers()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = base->vulkandevice->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)base->commandBuffers.size();

    imguiCommandBuffer.resize(base->swapChain->swapChainFramebuffers.size());
    if (vkAllocateCommandBuffers(base->device, &allocInfo, imguiCommandBuffer.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void VulkanImgui::createDescriptorPool()
{
    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    poolInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    poolInfo.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(base->device, &poolInfo, nullptr, &imguiDescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}
