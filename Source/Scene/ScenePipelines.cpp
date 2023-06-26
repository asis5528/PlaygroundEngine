#include "ScenePipelines.h"


ScenePipelines::ScenePipelines(VulkanBase *base,VkRenderPass renderPass) {

	this->base = base;
	
    GraphicsPipeline* opipe = new GraphicsPipeline();
    pipelines.push_back(opipe);
    PipelineInput input;
    input.vertexShaderpath = "shaders/vert.spv";
    input.fragmentShaderpath = "shaders/frag.spv";
    input.renderPass = renderPass;
    //   input.MultiviewLayer = frameBuffer[0].multiLayerView;
    input.device = base->device;
    input.msaaSamples = base->msaaSamples;
    input.viewportExtent = base->swapChain->swapChainExtent;
    input.descriptorTypes = { descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
        descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
        descriptorTypes{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT} };
    pipelines[0]->bufferSize = { sizeof(UniformBufferObject),sizeof(ubo1) };
    pipelines[0]->setupPipeline(input);
    pipelines[0]->createGraphicsPipeline();


    GraphicsPipeline* skinned = new GraphicsPipeline();
    pipelines.push_back(skinned);
    input.skinned = true;
    input.vertexShaderpath = "shaders/skinned.spv";
    pipelines[1]->bufferSize = { sizeof(SkinnedUniformBufferObject),sizeof(ubo1) };
    pipelines[1]->setupPipeline(input);
    pipelines[1]->createGraphicsPipeline();


    GraphicsPipeline* basic = new GraphicsPipeline();
    pipelines.push_back(basic);
    input.skinned = false;
    input.vertexShaderpath = "shaders/vert.spv";
    input.fragmentShaderpath = "shaders/Basic.spv";

    input.descriptorTypes = { descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
       descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_FRAGMENT_BIT} };
    pipelines[2]->bufferSize = { sizeof(UniformBufferObject) ,sizeof(BasicUbo) };
    pipelines[2]->setupPipeline(input);
    pipelines[2]->createGraphicsPipeline();
    float k = 1.;

    GraphicsPipeline* skinnedBasic = new GraphicsPipeline();
    pipelines.push_back(skinnedBasic);
    input.skinned = true;
    input.vertexShaderpath = "shaders/skinned.spv";
    pipelines[3]->bufferSize = { sizeof(SkinnedUniformBufferObject),sizeof(BasicUbo) };
    pipelines[3]->setupPipeline(input);
    pipelines[3]->createGraphicsPipeline();

    GraphicsPipeline* volumetric = new GraphicsPipeline();
    pipelines.push_back(volumetric);
    input.skinned = false;
    input.vertexShaderpath = "shaders/volumetricvert.spv";
    input.fragmentShaderpath = "shaders/volumetric.spv";

    input.descriptorTypes = { descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
         descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_FRAGMENT_BIT},
         descriptorTypes{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT} };
    pipelines[4]->bufferSize = { sizeof(UniformBufferObject),sizeof(BasicUbo) };
    pipelines[4]->setupPipeline(input);
    pipelines[4]->createGraphicsPipeline();
}
void ScenePipelines::recreate(VkRenderPass renderPass, VkExtent2D extent)
{
    for (GraphicsPipeline*& pipeline : pipelines) {
        pipeline->clean();
        pipeline->recreatePipeline(extent, renderPass);
    }
}


ScenePipelines::~ScenePipelines()
{
    for (GraphicsPipeline*& pipeline : pipelines) {
        delete pipeline;
    }
}
