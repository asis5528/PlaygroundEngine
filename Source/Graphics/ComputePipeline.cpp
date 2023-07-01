#include "ComputePipeline.h"
#include "VulkanUtils.h"


ComputePipeline::ComputePipeline(VulkanBase *base, ComputeInput input)
{

    this->base = base;
    this->m_input = input;



    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    for (uint32_t i = 0; i < input.descriptorTypes.size(); i++) {
        VkDescriptorSetLayoutBinding binding{};
        binding.binding = i;
        binding.descriptorCount = 1;
        binding.descriptorType = input.descriptorTypes[i];
        binding.pImmutableSamplers = nullptr;
        binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        layoutBindings.push_back(binding);
    }


    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
    layoutInfo.pBindings = layoutBindings.data();

    if (vkCreateDescriptorSetLayout(base->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    /*
    VkDescriptorSetLayoutBinding binding{};
    binding.binding = 0;
    binding.descriptorCount = 1;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    // binding.pImmutableSamplers = nullptr;
    binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;



    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;
    setLayoutBindings.push_back(binding);


    binding.binding = 1;
    binding.descriptorCount = 1;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    // binding.pImmutableSamplers = nullptr;
    binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    setLayoutBindings.push_back(binding);

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    layoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(base->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    */
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

    if (vkCreatePipelineLayout(base->device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }




    //loading and creating shaderstage is next


    auto computeShaderCode = utils::readFile(m_input.computeShaderPath);


    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = computeShaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(computeShaderCode.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(base->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }



    VkPipelineShaderStageCreateInfo shaderStage{};
    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStage.module = shaderModule;
    shaderStage.pName = "main";


    //
    VkComputePipelineCreateInfo computePipelineCreateInfo{};
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.layout = pipelineLayout;
    //  computePipelineCreateInfo.flags = 0;
    computePipelineCreateInfo.stage = shaderStage;

    if (vkCreateComputePipelines(base->device, nullptr, 1, &computePipelineCreateInfo, nullptr, &computePipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline!");
    }

   


    
}
ComputePipeline::~ComputePipeline() {

}
void ComputePipeline::setupDescriptors(std::vector <UBO> ubos, std::vector<VulkanTexture> textures, std::vector<SBO> sbos)
{


    base->vulkandescriptor->createDescriptorPool(m_input.descriptorTypes.size(),
        m_input.descriptorTypes,
        m_input.descriptorTypes.size()+1, &computeDescriptorPool);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = computeDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(1);
    allocInfo.pSetLayouts = &descriptorSetLayout;

    // descriptorSets.resize(size);
    if (vkAllocateDescriptorSets(base->device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }


    std::vector<VkWriteDescriptorSet> computeWriteDescriptorSets;
   // std::vector<VkWriteDescriptorSet> computeWriteDescriptorSets;

    computeWriteDescriptorSets.resize(textures.size()+ubos.size()+sbos.size());

    std::vector<VkDescriptorImageInfo> imageInfo{};
    imageInfo.resize(textures.size());
    int textureIndex = 0;

    std::vector<VkDescriptorBufferInfo> bufferInfo{};
    bufferInfo.resize(ubos.size());
    int uboIndex = 0;

    std::vector<VkDescriptorBufferInfo> sbobufferInfo{};
    sbobufferInfo.resize(sbos.size());
    int sboIndex = 0;

    for (int i = 0; i < m_input.descriptorTypes.size(); i++) {
        if (m_input.descriptorTypes[i] == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            bufferInfo[uboIndex].buffer = ubos[uboIndex].buffer;
            bufferInfo[uboIndex].offset = 0;
            bufferInfo[uboIndex].range = ubos[uboIndex].size;
            computeWriteDescriptorSets[i].pBufferInfo = &bufferInfo[uboIndex];
            uboIndex++;
        }
        else if (m_input.descriptorTypes[i] == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
            sbobufferInfo[sboIndex].buffer = sbos[sboIndex].buffer;
            sbobufferInfo[sboIndex].offset = 0;
            sbobufferInfo[sboIndex].range = sbos[sboIndex].size;
            computeWriteDescriptorSets[i].pBufferInfo = &sbobufferInfo[sboIndex];
            sboIndex++;
        }
        else {
            imageInfo[textureIndex].imageLayout = textures[textureIndex].imageLayout;
            imageInfo[textureIndex].imageView = textures[textureIndex].imageView;
            imageInfo[textureIndex].sampler = textures[textureIndex].imageSampler;
            computeWriteDescriptorSets[i].pImageInfo = &imageInfo[textureIndex];
            textureIndex++;

        }
        computeWriteDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        computeWriteDescriptorSets[i].dstSet = descriptorSet;
        computeWriteDescriptorSets[i].descriptorType = m_input.descriptorTypes[i];
        computeWriteDescriptorSets[i].dstBinding = i;
       // computeWriteDescriptorSets[i].pImageInfo = &imageInfo[i];
        computeWriteDescriptorSets[i].descriptorCount = 1;
        computeWriteDescriptorSets[i].dstArrayElement = 0;


    }
    /*
    for (int i = 0; i < textures.size(); i++) {

        
        imageInfo[i].imageLayout = textures[i].imageLayout;
        imageInfo[i].imageView = textures[i].imageView;
        imageInfo[i].sampler = textures[i].imageSampler;

      //  VkWriteDescriptorSet writeDescriptorSet{};
        computeWriteDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        computeWriteDescriptorSets[i].dstSet = descriptorSet;
        computeWriteDescriptorSets[i].descriptorType = m_input.descriptorTypes[i];
        computeWriteDescriptorSets[i].dstBinding = i;
        computeWriteDescriptorSets[i].pImageInfo = &imageInfo[i];
        computeWriteDescriptorSets[i].descriptorCount = 1;
        computeWriteDescriptorSets[i].dstArrayElement = 0;
        //computeWriteDescriptorSets.push_back(writeDescriptorSet);
    }*/
    /*
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo.imageView = ct3D.imageView;
    imageInfo.sampler = ct3D.imageSampler;

    VkDescriptorImageInfo imageInfo2{};
    imageInfo2.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo2.imageView = scene->textures[0].imageView;
    imageInfo2.sampler = scene->textures[0].imageSampler;

    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = descriptorSet;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writeDescriptorSet.dstBinding = 0;
    writeDescriptorSet.pImageInfo = &imageInfo;
    writeDescriptorSet.descriptorCount = 1;
    computeWriteDescriptorSets.push_back(writeDescriptorSet);
    
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = descriptorSet;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet.dstBinding = 1;
    writeDescriptorSet.pImageInfo = &imageInfo2;
    writeDescriptorSet.descriptorCount = 1;
    computeWriteDescriptorSets.push_back(writeDescriptorSet);

    */
    vkUpdateDescriptorSets(base->device, static_cast<uint32_t>(computeWriteDescriptorSets.size()), computeWriteDescriptorSets.data(), 0, nullptr);

}

void ComputePipeline::dispatch(uint32_t x, uint32_t y, uint32_t z)
{
    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex = base->vulkandevice->queueID;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (vkCreateCommandPool(base->device, &cmdPoolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute command pool!");
    }

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;
    if (vkAllocateCommandBuffers(base->device, &commandBufferAllocateInfo, &computeCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute command buffer!");
    }
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(base->device, &fenceCreateInfo, nullptr, &computeFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute fence!");
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(computeCommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    vkCmdBindPipeline(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
    vkCmdBindDescriptorSets(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, 0);

    vkCmdDispatch(computeCommandBuffer,x, y, z);

    if (vkEndCommandBuffer(computeCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
    float k = 0.;
}

void ComputePipeline::submit()
{

    vkWaitForFences(base->device, 1, &computeFence, VK_TRUE, UINT64_MAX);
    vkResetFences(base->device, 1, &computeFence);

    VkSubmitInfo computeSubmitInfo{};
    computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &computeCommandBuffer;


    if (vkQueueSubmit(base->queue, 1, &computeSubmitInfo, computeFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to sumbit queue!");
    }
}
