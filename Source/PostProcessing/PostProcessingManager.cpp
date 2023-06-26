#include "PostProcessingManager.h"

PostProcessingManager::PostProcessingManager(VulkanBase* base, Framebuffer* previousFB)
{
	this->base = base;
	this->previousFB = previousFB;
	volumeRendering = new VolumeRendering(base, previousFB);
	for (Framebuffer*& framebuffer : volumeRendering->framebuffers) {

		framebuffers.push_back(framebuffer);
	}
	bufferSize = framebuffers.size();
}

void PostProcessingManager::renderPass(VkCommandBuffer commandBuffer)
{
	volumeRendering->renderPass(commandBuffer);
}

void PostProcessingManager::recreate(VkExtent2D ex)
{
	volumeRendering->recreate(ex);
}

PostProcessingManager::~PostProcessingManager()
{
	delete volumeRendering;
}
