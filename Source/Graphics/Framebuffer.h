#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>
#include <array>
#include "VulkanImage.h"
#include "VulkanDevice.h"
class Framebuffer
{
	public:
		uint32_t width;
		uint32_t height;
		VulkanDevice *vulkandevice;
		//VkDevice device;
		//VkPhysicalDevice physicalDevice;
		VkSampleCountFlagBits msaaSamples;
		VkRenderPass renderPass;
		VkFormat colorFormat;
		VkFormat depthFormat;
		VulkanImage* ColorImage;
		VulkanImage* DepthImage;
		VulkanImage* MultisampledColorImage;
		VkFramebuffer framebuffer;

		Framebuffer(VulkanDevice *vulkandevice , uint32_t width, uint32_t height,VkSampleCountFlagBits msaaSamples,VkFormat colorFormat, VkFormat depthFormat);
		~Framebuffer();
		void recreate();
		void createRenderPass();
		void createFrameBuffer();
		void beginRenderPass(VkCommandBuffer commandBuffer);
};

