#include "MeshToSdf.h"


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
		//	std::vector<SBO> sbos;
			//sbos.push_back(sbo);
			//sbo.size = scene->meshes[1].ve
			comp->setupDescriptors({}, { computeTexture }, { sbo,sbo2 });
			comp->dispatch(computeTexture.width / 8, computeTexture.height / 8, computeTexture.depth / 8);
			comp->submit();
		}
		k = false;

	}
}
