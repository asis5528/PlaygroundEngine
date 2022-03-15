#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
namespace utils {


    VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);
     std::vector<char> readFile(const std::string& filename);
     uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);


}

