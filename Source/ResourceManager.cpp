#include "ResourceManager.h"
std::vector<VulkanTexture> ResourceManager::textureList{}; // Initialize the static vector
std::vector<VkSampler> ResourceManager::textureSamplerList{}; // Initialize the static vector
const VulkanBase* ResourceManager::base = nullptr;



__int64 ResourceManager::addTexture( VulkanTexture texture) {
    textureList.push_back(texture);
    return textureList.size()-1;
}

VulkanTexture ResourceManager::getTexture(int index)
{
    return textureList[index];
}

void ResourceManager::addTextureSampler(VkSampler sampler)
{
    textureSamplerList.push_back(sampler);
}

VkSampler ResourceManager::getTextureSampler(int index)
{
    return textureSamplerList[index];
}

void ResourceManager::setVulkanBase(const VulkanBase* vulkanBase)
{
    ResourceManager::base = vulkanBase;

}

void ResourceManager::cleanTextures()
{
    for (VulkanTexture& t : textureList) {
        vkDestroyImageView(base->device, t.imageView, nullptr);
        vkDestroyImage(base->device, t.image, nullptr);
        vkFreeMemory(base->device, t.imageMemory, nullptr);
    }
}

void ResourceManager::cleanTextureSamplers()
{
    for (VkSampler& s : textureSamplerList) {
        vkDestroySampler(base->device, s, nullptr);
    }
}
