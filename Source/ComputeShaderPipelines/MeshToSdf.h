#pragma once
#include "../Scene/Scene.h"
#include "../Graphics/ComputePipeline.h"
class MeshToSdf
{
public:
	std::vector<SBO> computeSBOVertices;
	VulkanTexture3D computeTexture;
	ComputePipeline* comp;
	VulkanBase* base;
	Scene* scene;
	MeshToSdf(VulkanBase* base, Scene* scene);
	void update(float time);
};

