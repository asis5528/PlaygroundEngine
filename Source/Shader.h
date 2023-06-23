#pragma once
#include "Graphics/GraphicsPipeline.h"
struct UBO {
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
	uint64_t size;
	void map(const VkDevice device, uint64_t size,void* ubo) {
		void* data;
		vkMapMemory(device, bufferMemory, 0, size, 0, &data);
		//void* k = &ubo;
		memcpy(data, ubo, size);
		vkUnmapMemory(device, bufferMemory);

		}

};
struct VulkanShader {
	GraphicsPipeline *graphicsPipeline;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;
	std::vector<UBO> ubos;

};

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view[2];
	alignas(16) glm::mat4 proj[2];
};

struct SkinnedUniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view[2];
	alignas(16) glm::mat4 proj[2];
	glm::mat4 boneMatrices[100] = { glm::mat4(1.0) };

};
struct ubo1 {
	float time1;
	float time2;
	float time3;
};

struct BasicUbo {
	glm::vec4 color;
};

