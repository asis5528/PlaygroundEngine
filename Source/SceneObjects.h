#pragma once
#include "Graphics/VulkanBase.h"
#include <vector>
#include "Object.h"
#include "quad.h"
class SceneObjects
{

public:
	std::vector<GraphicsPipeline*> pipelines;
	std::vector<VulkanTexture> textures;
	std::vector<Mesh> meshes;
	std::vector<Object> objects;
	std::vector<Quad*> quads;
	VkDescriptorPool descriptorPool;
	VulkanBase* base;
};

