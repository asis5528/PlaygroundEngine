#include "MeshToSdf.h"
#include "../ResourceManager.h"


MeshToSdf::MeshToSdf(VulkanBase* base, Scene* scene)
{
	this->base = base;
	this->scene = scene;
	int size = 128;
	computeTexture = base->vbuffer->createTexture3DGeneral(size, size, size);
	ComputeInput input;
	input.computeShaderPath = "shaders/ComputeShaders/MeshtoSdf.spv";
	input.descriptorTypes = { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
	comp = new ComputePipeline(base, input);
}
MeshToSdf::~MeshToSdf() {
	delete comp;
	//computeTexture.destroy(base->device);
	//vkDestroySampler(base->device, computeTexture.imageSampler, nullptr);
}
void MeshToSdf::update(float time)
{
	static bool k = true;
	if (scene->meshes.size() > 1) {
		if (scene->dispatch) {
			scene->dispatch = false;
			SBO sbo;
			sbo.buffer = scene->meshes[1].vertexBuffer;
			sbo.size = sizeof(scene->meshes[1].vertices[0]) * scene->meshes[1].vertices.size();

			SBO sbo2;
			sbo2.buffer = scene->meshes[1].indexBuffer;
			sbo2.size = sizeof(scene->meshes[1].indices[0]) * scene->meshes[1].indices.size();

			comp->setupDescriptors({}, { ResourceManager::getTexture(computeTexture.id) }, { sbo,sbo2 });
			comp->dispatch(computeTexture.width / 8, computeTexture.height / 8, computeTexture.depth / 8);
			comp->submit();
			//vkDestroyBuffer(base->device, sbo.buffer, nullptr);
		//	vkFreeMemory(base->device, sbo.bufferMemory, nullptr);
		}
		k = false;

	}
}
