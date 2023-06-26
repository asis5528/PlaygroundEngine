#pragma once
#include <vector>
#include "../Graphics/Framebuffer.h"
#include "../quad.h"
#include "VolumeRendering.h"
class PostProcessingManager
{
	public:
		std::vector<Quad*> quads;
		Framebuffer* previousFB;
		std::vector<Framebuffer*> framebuffers;
		VulkanBase* base;
		VolumeRendering *volumeRendering;
		int bufferSize;

		PostProcessingManager(VulkanBase *base,Framebuffer *previousFB);
		void renderPass(VkCommandBuffer commandBuffer);
		void recreate(VkExtent2D ex);
		~PostProcessingManager();
};

