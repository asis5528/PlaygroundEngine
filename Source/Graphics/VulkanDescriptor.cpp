#include "VulkanDescriptor.h"
#include <stdexcept>

VulkanDescriptor::VulkanDescriptor(VulkanDevice* vulkandevice) {
	this->vulkandevice = vulkandevice;
}

void VulkanDescriptor::createDescriptorPool(uint32_t size, std::vector<VkDescriptorType> descriptorTypes, uint32_t maxsets, VkDescriptorPool* descriptorPool)
{
	const uint32_t siz = descriptorTypes.size();
	std::vector<VkDescriptorPoolSize> poolSizes;
	poolSizes.resize(descriptorTypes.size());
	for (int i = 0; i < poolSizes.size(); i++) {
		poolSizes[i].type = descriptorTypes[i];
		poolSizes[i].descriptorCount = static_cast<uint32_t>(size+1);
	}
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = maxsets;

	if (vkCreateDescriptorPool(vulkandevice->device, &poolInfo, nullptr, descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}


}
std::vector<VkDescriptorSet> VulkanDescriptor::createDescriptorSets(uint32_t size, std::vector<VkDescriptorSetLayout> layouts, VkDescriptorPool descriptorPool) {
	//std::vector<VkDescriptorSetLayout> layouts(size, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(size);
	allocInfo.pSetLayouts = layouts.data();
	std::vector<VkDescriptorSet> descriptorSets;
	descriptorSets.resize(size);
	if (vkAllocateDescriptorSets(vulkandevice->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
	return descriptorSets;
}
void VulkanDescriptor::updateDescriptorSets(VkDescriptorSet descriptorSet, std::vector<UBO> ubos, std::vector<VulkanTexture> textures) {

	std::vector<VkWriteDescriptorSet> descriptorWrites;
	descriptorWrites.resize(ubos.size() + textures.size());
	int index = 0;
	std::vector< VkDescriptorBufferInfo> bufferInfos;
	for (int i = 0; i < ubos.size(); i++) {
		VkDescriptorBufferInfo binfo{};
		binfo.buffer = ubos[i].buffer;
		binfo.offset = 0;
		binfo.range = ubos[i].size;
		bufferInfos.push_back(binfo);
	}
	for (int i = 0; i < ubos.size(); i++) {

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = ubos[i].buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = ubos[i].size;

		descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[index].dstSet = descriptorSet;
		descriptorWrites[index].dstBinding = index;
		descriptorWrites[index].dstArrayElement = 0;
		descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[index].descriptorCount = 1;
		descriptorWrites[index].pBufferInfo = &bufferInfos[i];
		index++;
	}

	std::vector< VkDescriptorImageInfo> imageInfos;
	for (int i = 0; i < textures.size(); i++) {


		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = textures[i].imageLayout;
		imageInfo.imageView = textures[i].imageView;
		imageInfo.sampler = textures[i].imageSampler;
		imageInfos.push_back(imageInfo);

	}
	for (int i = 0; i < textures.size(); i++) {

		
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = textures[i].imageLayout;
		imageInfo.imageView = textures[i].imageView;
		imageInfo.sampler = textures[i].imageSampler;
		descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[index].dstSet = descriptorSet;
		descriptorWrites[index].dstBinding = index;
		descriptorWrites[index].dstArrayElement = 0;
		descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[index].descriptorCount = 1;
		descriptorWrites[index].pImageInfo = &imageInfos[i];
		index++;
	}

	vkUpdateDescriptorSets(vulkandevice->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}