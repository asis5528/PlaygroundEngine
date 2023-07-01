#pragma once
#include "Graphics/GraphicsPipeline.h"
#include "Graphics/VulkanBase.h"
//REMAINDER,MAKE THIS QUAD AS SEPARATE FOR POSTPROCESSING QUAD,MAYBE INHERTIANCE FROM BASIC QUAD CLASS
class Quad
{
public:
	GraphicsPipeline *graphicsPipeline;
	VulkanBase* base;
	VkDescriptorSet descriptorSet;
	VkDescriptorPool descriptorPool;
	std::vector<VulkanTexture> textures;
	Framebuffer *fr;
	Framebuffer* fr2;
	//Quad(VulkanBase *vbase);
	Quad(VulkanBase* vbase, Framebuffer *fr);
	~Quad();
	void createGraphicsPipeline(std::string vshaderpath, std::string fshaderpath, std::vector<descriptorTypes> descriptorTypes);
	void recreateGraphicsPipeline(VkExtent2D ex);
	void createdescriptors();
	void cleandescriptors();
	void draw(const VkCommandBuffer& commandBuffer);

};

