#include "GaussianFluidSim.h"


struct fluidUBO {
	float time;
	int Frame;

};
GaussianFluidSim::GaussianFluidSim(VulkanBase* base, Scene* scene, VulkanTexture3D sdftex)
{
	this->base = base;
	this->scene = scene;
	int size = 128;
	computeTexture = base->vbuffer->createTexture3DGeneral(size, size, size);
	computeTexture2 = base->vbuffer->createTexture3DGeneral(size, size, size);

	computeUBO = base->vbuffer->createUniformBuffers({ sizeof(fluidUBO) });
	ComputeInput input;
	input.computeShaderPath = "shaders/ComputeShaders/GaussianFluidSim.spv";
	input.descriptorTypes = { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER };
	comp = new ComputePipeline(base, input);
	comp->setupDescriptors(computeUBO, { computeTexture,scene->textures[0],computeTexture2,sdftex }, {});
	comp->dispatch(computeTexture.width / 8, computeTexture.height / 8, computeTexture.depth / 8);

	

	computeUBO2 = base->vbuffer->createUniformBuffers({ sizeof(fluidUBO) });
	ComputeInput input2;
	input2.computeShaderPath = "shaders/ComputeShaders/GaussianFluidSim.spv";
	input2.descriptorTypes = { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER };
	comp2 = new ComputePipeline(base, input2);
	comp2->setupDescriptors(computeUBO, { computeTexture2,scene->textures[0],computeTexture,sdftex }, {});
	comp2->dispatch(computeTexture2.width / 8, computeTexture2.height / 8, computeTexture2.depth / 8);

}

void GaussianFluidSim::update(float time)
{
	static int i = 0;
	fluidUBO fubo;
	fubo.time = time;
	if(i<10){
		computeUBO[0].map(base->device, sizeof(fluidUBO), &fubo);
		comp->submit();

		computeUBO2[0].map(base->device, sizeof(fluidUBO), &fubo);
		comp2->submit();
		}
	//i++;
}
