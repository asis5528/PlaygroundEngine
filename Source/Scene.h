#pragma once
#include "Graphics/VulkanBase.h"
#include "Object.h"
#include "quad.h"
class Scene
{
	
public:
	std::vector<Texture> textures;
	std::vector<Mesh> meshes;
	std::vector<Object> objects;
	GraphicsPipeline* objectPipeline;
	VkDescriptorPool descriptorPool;
	VulkanBase* base;
	Quad* quad;
	Scene(VulkanBase *vbase);
};

