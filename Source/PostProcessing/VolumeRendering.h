#pragma once
#include <vector>
#include "../Graphics/Framebuffer.h"
#include "../quad.h"
class VolumeRendering
{
	public:
		std::vector<Quad*> quads;
		std::vector<Framebuffer*> framebuffers;
		VulkanBase *base;
		Framebuffer* previousFB;
		VolumeRendering(VulkanBase* base, Framebuffer* previousFB);
		void renderPass(VkCommandBuffer commandBuffer);
		void recreate(VkExtent2D ex);
		~VolumeRendering();
};

