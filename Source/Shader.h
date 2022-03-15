#pragma once
#include "Graphics/GraphicsPipeline.h"
struct UBO {
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
	uint64_t size;
	void map(const VkDevice *device, uint64_t size,void* ubo) {
		void* data;
		vkMapMemory(*device, bufferMemory, 0, size, 0, &data);
		//void* k = &ubo;
		memcpy(data, ubo, size);
		vkUnmapMemory(*device, bufferMemory);

		}

};
struct Shader {
	GraphicsPipeline *graphicsPipeline;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;
	std::vector<UBO> ubos;

};