#pragma once
#include <vector>
#include "Graphics/VulkanImage.h"
#include "Graphics/VulkanBase.h"
class VulkanBase;
class ResourceManager {
public:

    static __int64 addTexture( VulkanTexture texture);
    static VulkanTexture getTexture(int index);
    static void addTextureSampler(VkSampler sampler);
    static VkSampler getTextureSampler(int index);
    static void setVulkanBase(const VulkanBase* vulkanBase);
    static const VulkanBase* base;
    static void cleanTextures(); 
    static void cleanTextureSamplers();
    
private:
    
    static std::vector<VulkanTexture> textureList;
    static std::vector<VkSampler> textureSamplerList;
};
