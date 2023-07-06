#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "GraphicsPipeline.h"
#include "Mesh.h"
#include "../Shader.h"
#include "VulkanImage.h"
#include "VulkanDevice.h"
#include "../PGEResourceTypes.h"

struct VBuffer {
	VulkanDevice* vulkandevice;
	VBuffer(VulkanDevice* vulkandevice);
	void createVertexBuffer(Mesh& mesh); 
	void createSkinnedVertexBuffer(Mesh& mesh);
	void createIndexBuffer(Mesh& mesh);
	std::vector<UBO> createUniformBuffers(std::vector<uint64_t> Buffersize);
//	std::vector 
	PGEResourceTypes::Texture createTexturefromBuffer(VulkanTexture& texture, void* pixels);
	PGEResourceTypes::Texture3D createTexture3DGeneral(int w,int h,int d);
	PGEResourceTypes::Texture3D createTexture3DfromBuffer(VulkanTexture3D& texture, void* pixels);
	void transitionImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,uint32_t depth);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	VkSampler createTextureSampler(uint32_t miplevels);

};



