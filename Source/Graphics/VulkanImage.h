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
	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
	void createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	
private:
	uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

};

namespace Image {
	VkImageView createImageView(VkDevice device, VkPhysicalDevice physicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void createImage(VkDevice device,VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

}

