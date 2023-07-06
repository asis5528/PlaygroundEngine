#pragma once
#include <vulkan/vulkan.h>
class VulkanImage
{
public:
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkDeviceMemory imageMemory;
	VkImage image;
	VkImageView imageView;
	VulkanImage(VkDevice device, VkPhysicalDevice physicalDevice);
	void destroy();
//	void create(VkDevice device, VkPhysicalDevice physicalDevice);
	void createImage(uint32_t width, uint32_t height,uint32_t depth, VkImageType type ,uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t arrayCount = 1);
	void createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, VkImageViewType type, uint32_t layerCount = 1);

	VkSampler createTextureSampler(uint32_t miplevels);
	
private:
	uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

};
struct VulkanTexture {
	uint32_t mipLevels;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	VkImage image;
	VkDeviceMemory imageMemory;
	VkImageView imageView;
	VkSampler imageSampler;
	//VulkanImage vimage;
	VkImageType imageType = VK_IMAGE_TYPE_2D;
	VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D;
	VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	void destroy(VkDevice device);


};
struct VulkanTexture3D : VulkanTexture {

	//  VkImageType imageType = VK_IMAGE_TYPE_3D;
	//  VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_3D;
};

namespace Image {
	VkImageView createImageView(VkDevice device, VkPhysicalDevice physicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void createImage(VkDevice device,VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

}

