#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "GraphicsPipeline.h"
#include "Mesh.h"
#include "../Shader.h"
#include "VulkanDevice.h"
struct VBuffer {
	VulkanDevice* vulkandevice;
	VBuffer(VulkanDevice* vulkandevice);
	void createVertexBuffer(Mesh& mesh);
	void createIndexBuffer(Mesh& mesh);
	std::vector<UBO> createUniformBuffers(std::vector<uint64_t> Buffersize);
//	std::vector 
	void createTexturefromBuffer(Texture& texture, void* pixels);
	void transitionImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	VkSampler createTextureSampler(uint32_t miplevels);

};



