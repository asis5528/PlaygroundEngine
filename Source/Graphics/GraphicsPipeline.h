#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <array>
#include <string>
#include <vector>
#include "Mesh.h"


   
    struct descriptorTypes {
        VkDescriptorType descriptorType;
        VkShaderStageFlagBits shaderStage;
    };

    struct PipelineInput {
        std::string vertexShaderpath;
        std::string fragmentShaderpath;
        VkRenderPass renderPass;
        VkExtent2D viewportExtent;
        VkSampleCountFlagBits msaaSamples;
        
        VkDevice device;
      //  VkPipelineCache pipelineCache;
        std::vector<descriptorTypes> descriptorTypes;

    };
    struct PipelineInfoLocal {       
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        std::vector<VkPipelineShaderStageCreateInfo> shaderStagess;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        VkVertexInputBindingDescription bindingDescription;
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions;
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        VkViewport viewport{};
        VkRect2D scissor{};
        VkPipelineViewportStateCreateInfo viewportState{};
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineMultisampleStateCreateInfo multisampling{};
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo colorBlending{};

    };
class GraphicsPipeline
{
    struct UniformBufferObject {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };
public:
    PipelineInput pipelineInput;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkGraphicsPipelineCreateInfo pipelineInfo ;
    VkPipeline graphicsPipeline;
    PipelineInfoLocal infoLocal;
    std::vector<uint64_t> bufferSize;
   // void k(PipelineInput* pipelineInput);
    ~GraphicsPipeline();
    void setupPipeline(PipelineInput pipelineInput);
    void recreatePipeline(VkExtent2D resolution,VkRenderPass renderPass);
    void createGraphicsPipeline();
private:
    void createDescriptorSetLayout();
    VkShaderModule createShaderModule(const std::vector<char>& code);
};

