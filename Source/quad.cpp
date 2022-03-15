#include "quad.h"

Quad::Quad(VulkanBase* vbase)
{
	this->base = vbase;
}

Quad::~Quad() {
    vkDestroyDescriptorPool(base->device, descriptorPool, nullptr);
    //delete graphicsPipeline;
}

void Quad::createGraphicsPipeline(std::string vshaderpath, std::string fshaderpath)
{
    graphicsPipeline = new GraphicsPipeline();
    PipelineInput input;
    input.device = base->device;
    input.vertexShaderpath = vshaderpath;
    input.fragmentShaderpath = fshaderpath;
    input.renderPass = base->swapChain->renderPass;
    input.msaaSamples = base->msaaSamples;
    input.viewportExtent = base->swapChain->swapChainExtent;
    input.descriptorTypes = { descriptorTypes{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT} };
    graphicsPipeline->setupPipeline(input);
    VkPipelineVertexInputStateCreateInfo vi{};
    vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vi.pVertexAttributeDescriptions = nullptr;
    vi.pVertexBindingDescriptions = nullptr;
    vi.vertexAttributeDescriptionCount = 0;
    vi.vertexBindingDescriptionCount = 0;
    graphicsPipeline->infoLocal.vertexInputInfo = vi;
    // quadPipeline->pipelineInfo.pVertexInputState = &vi;

    graphicsPipeline->createGraphicsPipeline();

}
void Quad::draw(const VkCommandBuffer& commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

void Quad::createdescriptors()
{
    std::vector<VkDescriptorType> types;
    for (int i = 0; i < graphicsPipeline->pipelineInput.descriptorTypes.size(); i++) {
        types.push_back(graphicsPipeline->pipelineInput.descriptorTypes[i].descriptorType);
   }
    base->vulkandescriptor->createDescriptorPool(1,
        types,
       2, &descriptorPool);

    std::vector<VkDescriptorSetLayout> Qlayouts(1, graphicsPipeline->descriptorSetLayout);
    descriptorSet = base->vulkandescriptor->createDescriptorSets(1, Qlayouts, descriptorPool)[0];


    base->vulkandescriptor->updateDescriptorSets(descriptorSet, {  }, textures);
}

void Quad::cleandescriptors()
{
    vkDestroyDescriptorPool(base->device, descriptorPool, nullptr);
}
