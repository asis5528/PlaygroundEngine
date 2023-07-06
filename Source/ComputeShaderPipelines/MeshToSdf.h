#pragma once
#include "../Scene/Scene.h"
#include "../Graphics/ComputePipeline.h"
class MeshToSdf
{
public:
	std::vector<SBO> computeSBOVertices;
	PGEResourceTypes::Texture3D computeTexture;
	ComputePipeline* comp;
	VulkanBase* base;
	Scene* scene;
	MeshToSdf(VulkanBase* base, Scene* scene);
	~MeshToSdf();
	void update(float time);
};

