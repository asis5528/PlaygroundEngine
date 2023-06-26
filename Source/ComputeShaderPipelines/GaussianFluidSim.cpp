#include "GaussianFluidSim.h"


struct fluidUBO {
	float time;
	int Frame;

};
GaussianFluidSim::GaussianFluidSim(VulkanBase* base, Scene* scene)
{
	this->base = base;
	this->scene = scene;
	int size = 128;
	computeTexture = base->vbuffer->createTexture3DGeneral(size, size, size);

	computeUBO = base->vbuffer->createUniformBuffers({ sizeof(fluidUBO) });
	ComputeInput input;
	input.computeShaderPath = "shaders/GaussianFluidSim.spv";
	input.descriptorTypes = { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
	comp = new ComputePipeline(base, input);
	comp->setupDescriptors(computeUBO, { computeTexture,scene->textures[0] });
	comp->dispatch(computeTexture.width / 8, computeTexture.height / 8, computeTexture.depth / 8);
}

void GaussianFluidSim::update(float time)
{
	fluidUBO fubo;
	fubo.time = time;
	computeUBO[0].map(base->device, sizeof(fluidUBO), &fubo);
	comp->submit();
}
