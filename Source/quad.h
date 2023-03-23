#pragma once
#include "Graphics/GraphicsPipeline.h"
#include "Graphics/VulkanBase.h"

class Quad
{
public:
	GraphicsPipeline *graphicsPipeline;
	VulkanBase* base;
	VkDescriptorSet descriptorSet;
	VkDescriptorPool descriptorPool;
	std::vector<VulkanTexture> textures;
	Framebuffer *fr;
	//Quad(VulkanBase *vbase);
	Quad(VulkanBase* vbase, Framebuffer *fr);
	~Quad();
	void createGraphicsPipeline(std::string vshaderpath, std::string fshaderpath);
	void createdescriptors();
	void cleandescriptors();
	void draw(const VkCommandBuffer& commandBuffer);

};

