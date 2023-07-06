#include "Framebuffer.h"
#include "VulkanUtils.h"
#include <stdexcept>
//REMEMBER TO NO APPLY MULTISAMPLED COLOR FOR NO MSAA SAMPLES FRAMEBUFFERS FOR OPTIMIZATION
Framebuffer::Framebuffer(VulkanDevice *vulkandevice , uint32_t width, uint32_t height, VkSampleCountFlagBits msaaSamples, VkFormat colorFormat, VkFormat depthFormat,int MultiLayerView) {
	this->width = width;
	this->height = height;
    this->vulkandevice = vulkandevice;
    this->multiLayerView = MultiLayerView;
   // this->device = device;
    //this->physicalDevice = physicalDevice;
    this->msaaSamples = msaaSamples;
    this->colorFormat = colorFormat;
    this->depthFormat = depthFormat;
    recreate();
}
Framebuffer::~Framebuffer()
{
    clean();
}
void Framebuffer::clean() {
    ColorImage->destroy();
    DepthImage->destroy();
    MultisampledColorImage->destroy();
    MultisampledDepthImage->destroy();
    vkDestroyFramebuffer(vulkandevice->device, framebuffer, nullptr);
    vkDestroyRenderPass(vulkandevice->device, renderPass, nullptr);
}
void Framebuffer::recreate() {
    createRenderPass();
    createFrameBuffer();
}
void Framebuffer::createRenderPass() {
    /*
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = colorFormat;
    colorAttachment.samples = msaaSamples;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = depthFormat;
    depthAttachment.samples = msaaSamples;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    
    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = colorFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription depthAttachmentResolve{};
    depthAttachmentResolve.format = depthFormat;
    depthAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;


    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference2 depthAttachmentResolveRef{};
    depthAttachmentResolveRef.attachment = 3;
    depthAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescriptionDepthStencilResolve st;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    VkSubpassDescriptionDepthStencilResolve DSR;
    DSR.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE;
    DSR.pNext = nullptr;
    DSR.depthResolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
    DSR.stencilResolveMode = VK_RESOLVE_MODE_NONE;
    DSR.pDepthStencilResolveAttachment = &depthAttachmentResolveRef;

    
    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


    std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = dependencies.size();
    renderPassInfo.pDependencies = dependencies.data();
    if(multiLayerView>1){
        const uint32_t viewMask = 0b00000011;

       
        const uint32_t correlationMask = 0b00000011;

        VkRenderPassMultiviewCreateInfo renderPassMultiviewCI{};
        renderPassMultiviewCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO;
        renderPassMultiviewCI.subpassCount = 1;
        renderPassMultiviewCI.pViewMasks = &viewMask;
        renderPassMultiviewCI.correlationMaskCount = 1;
        renderPassMultiviewCI.pCorrelationMasks = &correlationMask;

     //   renderPassInfo.pNext = &renderPassMultiviewCI;
    }
    if (vkCreateRenderPass(vulkandevice->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
    */
      VkAttachmentDescription2 colorAttachment{}; 
    colorAttachment.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
    colorAttachment.format = colorFormat;
    colorAttachment.samples = msaaSamples;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    colorAttachment.pNext = nullptr;
    

    VkAttachmentDescription2 depthAttachment{}; 
    depthAttachment.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
    depthAttachment.format = depthFormat;
    depthAttachment.samples = msaaSamples;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    depthAttachment.pNext = nullptr;
    
    VkAttachmentDescription2 colorAttachmentResolve{};
    colorAttachmentResolve.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
    colorAttachmentResolve.format = colorFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    colorAttachmentResolve.pNext = nullptr;

    VkAttachmentDescription2 depthAttachmentResolve{};
    depthAttachmentResolve.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
    depthAttachmentResolve.format = depthFormat;
    depthAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    depthAttachmentResolve.pNext = nullptr;

    VkAttachmentReference2 colorAttachmentRef{};
    colorAttachmentRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachmentRef.pNext = nullptr;

    VkAttachmentReference2 depthAttachmentRef{};
    depthAttachmentRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachmentRef.pNext = nullptr;

    VkAttachmentReference2 colorAttachmentResolveRef{};
    colorAttachmentResolveRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachmentResolveRef.pNext = nullptr;

    VkAttachmentReference2 depthAttachmentResolveRef{};
    depthAttachmentResolveRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
    depthAttachmentResolveRef.attachment = 3;
    depthAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachmentResolveRef.pNext = nullptr;

    VkSubpassDescriptionDepthStencilResolve st;
    VkSubpassDescription ssss{};
    VkSubpassDescription2 subpass{};
    subpass.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;
    subpass.pInputAttachments = nullptr;
    subpass.preserveAttachmentCount = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;
    subpass.pNext = nullptr;
    
    VkSubpassDescriptionDepthStencilResolve DSR;
    DSR.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE;
    DSR.pNext = nullptr;
    DSR.depthResolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
    DSR.stencilResolveMode = VK_RESOLVE_MODE_NONE;
    DSR.pDepthStencilResolveAttachment = &depthAttachmentResolveRef;
    subpass.pNext = &DSR;
    //VkSubpassDescription2 ss{};
    
    
    std::array<VkSubpassDependency2, 2> dependencies;
    dependencies[0].sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2;
    dependencies[0].pNext = nullptr;
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    dependencies[0].viewOffset = 0;

    dependencies[1].srcSubpass = 0;
    dependencies[1].pNext = nullptr;
    dependencies[1].sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    dependencies[1].viewOffset = 0;

    std::array<VkAttachmentDescription2, 4> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve ,depthAttachmentResolve};
   
    VkRenderPassCreateInfo2 renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = dependencies.size();
    renderPassInfo.pDependencies = dependencies.data();
   
    //renderPassInfo.pCorrelatedViewMasks = nullptr;
    //renderPassInfo.correlatedViewMaskCount = 1;
    
    renderPassInfo.pNext = nullptr;
    if(multiLayerView>1){
        const uint32_t viewMask = 0b00000011;

    const uint32_t correlationMask = 0b00000011;

    VkRenderPassMultiviewCreateInfo renderPassMultiviewCI{};
    renderPassMultiviewCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO;
    renderPassMultiviewCI.subpassCount = 1;
    renderPassMultiviewCI.pViewMasks = &viewMask;
    renderPassMultiviewCI.correlationMaskCount = 1;
    renderPassMultiviewCI.pCorrelationMasks = &correlationMask;
    //renderPassInfo.pCorrelatedViewMasks = &correlationMask;
  //  renderPassInfo.correlatedViewMaskCount = 1;

    //   renderPassInfo.pNext = &renderPassMultiviewCI;
    }
    if (vkCreateRenderPass2(vulkandevice->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
    
    
}

void Framebuffer::createFrameBuffer() {
    //Coz of multiview for vr,layer is assigned 2 and imageview type is a 2D array...change the code to make compatible for anytype of framebuffer textures in future
   
    //int multiviewLayer = 2;
    VkImageViewType viewType;
    if (multiLayerView >1) {
        viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    }
    else {
        viewType = VK_IMAGE_VIEW_TYPE_2D;
    }
    ColorImage = new VulkanImage(vulkandevice->device, vulkandevice->physicalDevice);
    ColorImage->createImage(width, height,1, VK_IMAGE_TYPE_2D,1, this->msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, multiLayerView);
    ColorImage->createImageView(colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, viewType, multiLayerView);

    MultisampledColorImage = new VulkanImage(vulkandevice->device, vulkandevice->physicalDevice);
    MultisampledColorImage->createImage(width, height,1, VK_IMAGE_TYPE_2D, 1, VK_SAMPLE_COUNT_1_BIT, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, multiLayerView);
    MultisampledColorImage->createImageView(colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, viewType, multiLayerView);
    
    DepthImage = new VulkanImage(vulkandevice->device, vulkandevice->physicalDevice);
    DepthImage->createImage(width, height,1, VK_IMAGE_TYPE_2D, 1,this->msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, multiLayerView);
    DepthImage->createImageView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, viewType, multiLayerView);

    MultisampledDepthImage = new VulkanImage(vulkandevice->device, vulkandevice->physicalDevice);
    MultisampledDepthImage->createImage(width, height, 1, VK_IMAGE_TYPE_2D, 1, VK_SAMPLE_COUNT_1_BIT, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, multiLayerView);
    MultisampledDepthImage->createImageView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, viewType, multiLayerView);

    std::array<VkImageView, 4>attachments = { ColorImage->imageView,DepthImage->imageView,MultisampledColorImage->imageView,MultisampledDepthImage->imageView };
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(vulkandevice->device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create framebuffer!");
    }


}
void Framebuffer::beginRenderPass(VkCommandBuffer commandBuffer) {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = this->renderPass;
    renderPassInfo.framebuffer = this->framebuffer;
    // renderPassInfo.framebuffer = swapChainFramebuffers[i];
    renderPassInfo.renderArea.offset = { 0,0 };
    VkExtent2D resolution;
    resolution.width = width;
    resolution.height = height;
    renderPassInfo.renderArea.extent = resolution;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.8f, 0.8f, 0.8f, 1.8f} };
    clearValues[1].depthStencil = { 1.0f, 0 };


    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
}
