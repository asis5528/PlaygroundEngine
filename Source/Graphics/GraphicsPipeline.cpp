#include "GraphicsPipeline.h"
#include "VulkanUtils.h"
#include <stdexcept>


void GraphicsPipeline::setupPipeline(PipelineInput  pipelineInput) {
    this->pipelineInput = pipelineInput;
    auto vertShaderCode = utils::readFile(pipelineInput.vertexShaderpath);
    auto fragShaderCode = utils::readFile(pipelineInput.fragmentShaderpath);
    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    //infoLocal.vertShaderStageInfo{};
    infoLocal.vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    infoLocal.vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    infoLocal.vertShaderStageInfo.module = vertShaderModule;
    infoLocal.vertShaderStageInfo.pName = "main";

    //static VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    infoLocal.fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    infoLocal.fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    infoLocal.fragShaderStageInfo.module = fragShaderModule;
    infoLocal.fragShaderStageInfo.pName = "main";

    //static VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
    infoLocal.shaderStagess = { infoLocal.vertShaderStageInfo, infoLocal.fragShaderStageInfo };
    // static VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    infoLocal.vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // static VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
    // static std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = Vertex::getAttributeDescriptions();
    if (pipelineInput.skinned) {
        infoLocal.bindingDescription = SkinnedVertex::getBindingDescription();
        infoLocal.attributeDescriptions = SkinnedVertex::getAttributeDescriptions();
    }
    else {
        infoLocal.bindingDescription = Vertex::getBindingDescription();
        infoLocal.attributeDescriptions = Vertex::getAttributeDescriptions();
    }
  
    infoLocal.vertexInputInfo.vertexBindingDescriptionCount = 1;
    infoLocal.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(infoLocal.attributeDescriptions.size());
    infoLocal.vertexInputInfo.pVertexBindingDescriptions = &infoLocal.bindingDescription;
    infoLocal.vertexInputInfo.pVertexAttributeDescriptions = infoLocal.attributeDescriptions.data();



    // static VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    infoLocal.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    infoLocal.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    infoLocal.inputAssembly.primitiveRestartEnable = VK_FALSE;

    //  static VkViewport viewport{};
    infoLocal.viewport.x = 0.0f;
    infoLocal.viewport.y = 0.0f;
    infoLocal.viewport.width = (float)pipelineInput.viewportExtent.width;
    infoLocal.viewport.height = (float)pipelineInput.viewportExtent.height;
    infoLocal.viewport.minDepth = 0.0f;
    infoLocal.viewport.maxDepth = 1.0f;



    //static VkRect2D scissor{};
    infoLocal.scissor.offset = { 0, 0 };
    infoLocal.scissor.extent = pipelineInput.viewportExtent;
    // static VkPipelineViewportStateCreateInfo viewportState{};
    infoLocal.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    infoLocal.viewportState.viewportCount = 1;
    infoLocal.viewportState.pViewports = &infoLocal.viewport;
    infoLocal.viewportState.scissorCount = 1;
    infoLocal.viewportState.pScissors = &infoLocal.scissor;

    //static VkPipelineRasterizationStateCreateInfo rasterizer{};
    infoLocal.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    infoLocal.rasterizer.depthClampEnable = VK_FALSE;
    infoLocal.rasterizer.rasterizerDiscardEnable = VK_FALSE;
    infoLocal.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    infoLocal.rasterizer.lineWidth = 1.0f;
    infoLocal.rasterizer.cullMode = VK_CULL_MODE_NONE;
    infoLocal.rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    infoLocal.rasterizer.depthBiasEnable = VK_FALSE;




    //static VkPipelineMultisampleStateCreateInfo multisampling{};
    infoLocal.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    infoLocal.multisampling.sampleShadingEnable = VK_FALSE;
    infoLocal.multisampling.rasterizationSamples = pipelineInput.msaaSamples;

    // static VkPipelineDepthStencilStateCreateInfo depthStencil{};
    infoLocal.depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    infoLocal.depthStencil.depthTestEnable = VK_TRUE;
    infoLocal.depthStencil.depthWriteEnable = VK_TRUE;
    infoLocal.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    infoLocal.depthStencil.depthBoundsTestEnable = VK_FALSE;
    infoLocal.depthStencil.stencilTestEnable = VK_FALSE;

    //static VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    infoLocal.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    infoLocal.colorBlendAttachment.blendEnable = VK_TRUE;
    infoLocal.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    infoLocal.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    infoLocal.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    infoLocal.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    infoLocal.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    infoLocal.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    //static VkPipelineColorBlendStateCreateInfo colorBlending{};
    infoLocal.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    infoLocal.colorBlending.logicOpEnable = VK_FALSE;
    infoLocal.colorBlending.logicOp = VK_LOGIC_OP_COPY;
    infoLocal.colorBlending.attachmentCount = 1;
    infoLocal.colorBlending.pAttachments = &infoLocal.colorBlendAttachment;
    infoLocal.colorBlending.blendConstants[0] = 0.0f;
    infoLocal.colorBlending.blendConstants[1] = 0.0f;
    infoLocal.colorBlending.blendConstants[2] = 0.0f;
    infoLocal.colorBlending.blendConstants[3] = 0.0f;

    //this->pipelineInput = pipelineInput;
    createDescriptorSetLayout();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

    if (vkCreatePipelineLayout(pipelineInput.device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

}
GraphicsPipeline::~GraphicsPipeline() {
    clean();
    vkDestroyShaderModule(pipelineInput.device, infoLocal.vertShaderStageInfo.module, nullptr);
    vkDestroyShaderModule(pipelineInput.device, infoLocal.fragShaderStageInfo.module, nullptr);
    vkDestroyDescriptorSetLayout(pipelineInput.device, descriptorSetLayout, nullptr);
    vkDestroyPipelineLayout(pipelineInput.device, pipelineLayout, nullptr);
    
}
void GraphicsPipeline ::recreatePipeline(VkExtent2D resolution, VkRenderPass renderPass) {
    pipelineInput.viewportExtent = resolution;
    pipelineInput.renderPass = renderPass;
    infoLocal.viewport.width = (float)resolution.width;
    infoLocal.viewport.height = (float)resolution.height;
    infoLocal.scissor.extent = resolution;
    createGraphicsPipeline();
}
void GraphicsPipeline::clean()
{
    vkDestroyPipeline(pipelineInput.device, graphicsPipeline, nullptr);
   
}
void GraphicsPipeline::createGraphicsPipeline() {
    

    pipelineInfo.pVertexInputState = &infoLocal.vertexInputInfo;
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = infoLocal.shaderStagess.data();
    pipelineInfo.pInputAssemblyState = &infoLocal.inputAssembly;
    pipelineInfo.pViewportState = &infoLocal.viewportState;
    pipelineInfo.pRasterizationState = &infoLocal.rasterizer;
    pipelineInfo.pMultisampleState = &infoLocal.multisampling;
    pipelineInfo.pDepthStencilState = &infoLocal.depthStencil;
    infoLocal.colorBlending.pAttachments = &infoLocal.colorBlendAttachment;
    pipelineInfo.pColorBlendState = &infoLocal.colorBlending;
    
   
    pipelineInfo.renderPass = pipelineInput.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.layout = pipelineLayout;
    VkResult f = vkCreateGraphicsPipelines(pipelineInput.device, nullptr, 1, &pipelineInfo, nullptr, &graphicsPipeline);
    //infoLocal = {};
    if (f != VK_SUCCESS) {
        assert(f);
        throw std::runtime_error("failed to create graphics pipeline!");
    }


   
}
void GraphicsPipeline::createDescriptorSetLayout() {


    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    for (uint32_t i = 0; i < pipelineInput.descriptorTypes.size(); i++) {
        VkDescriptorSetLayoutBinding binding{};
        binding.binding = i;
        binding.descriptorCount = 1;
        binding.descriptorType = pipelineInput.descriptorTypes[i].descriptorType;
        binding.pImmutableSamplers = nullptr;
        binding.stageFlags = pipelineInput.descriptorTypes[i].shaderStage;
        layoutBindings.push_back(binding);
    }
   

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
    layoutInfo.pBindings = layoutBindings.data();

    if (vkCreateDescriptorSetLayout(pipelineInput.device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

  
}

VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(pipelineInput.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

VkVertexInputBindingDescription Vertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}
std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
    attributeDescriptions.resize(3);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, Vertex::pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, Vertex::normal);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, Vertex::texCoord);

    return attributeDescriptions;

}

VkVertexInputBindingDescription SkinnedVertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(SkinnedVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}
std::vector<VkVertexInputAttributeDescription> SkinnedVertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
    attributeDescriptions.resize(5);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(SkinnedVertex, SkinnedVertex::pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(SkinnedVertex, SkinnedVertex::normal);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(SkinnedVertex, SkinnedVertex::texCoord);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_UINT;;
    attributeDescriptions[3].offset = offsetof(SkinnedVertex, SkinnedVertex::IDs);

    attributeDescriptions[4].binding = 0;
    attributeDescriptions[4].location = 4;
    attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[4].offset = offsetof(SkinnedVertex, SkinnedVertex::Weights);

    return attributeDescriptions;

}

