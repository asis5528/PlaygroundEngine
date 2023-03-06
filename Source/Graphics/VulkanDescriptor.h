#pragma once
#include "VulkanDevice.h"
#include <array>
#include <vector>
#include "Mesh.h"
#include "../Shader.h"
#include "GraphicsPipeline.h"



struct VulkanDescriptor
{
	VulkanDevice * vulkandevice;
	VulkanDescriptor (VulkanDevice *vulkandevice);
	void createDescriptorPool(uint32_t size,std::vector<VkDescriptorType> descriptorTypes,  uint32_t maxsets,VkDescriptorPool * descriptorPool);
	std::vector<VkDescriptorSet> createDescriptorSets(uint32_t size, std::vector<VkDescriptorSetLayout> layouts,VkDescriptorPool descriptorPool);
	void updateDescriptorSets(VkDescriptorSet descriptorSet,std::vector<UBO> ubos,std::vector<VulkanTexture> textures);
}
;

