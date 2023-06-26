#pragma once
#include "VulkanBase.h"
#include "../Scene/Scene.h"

struct ComputeInput {
    std::string computeShaderPath;
    std::vector<VkDescriptorType> descriptorTypes;

};
class ComputePipeline
{

public:
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSet descriptorSet;
    VkPipeline computePipeline;
    VkCommandPool commandPool;
    VkDescriptorPool computeDescriptorPool;
    VkCommandBuffer computeCommandBuffer;
    VkFence computeFence;
    VulkanBase *base;


    ComputeInput m_input;
     ComputePipeline( VulkanBase* base, ComputeInput input);
     ~ComputePipeline();
     void setupDescriptors(std::vector <UBO> ubos, std::vector<VulkanTexture> textures);
     void dispatch(uint32_t group_x, uint32_t group_y, uint32_t group_z);
     void submit();
};

