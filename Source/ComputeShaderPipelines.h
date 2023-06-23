#pragma once
#include "Graphics/VulkanBase.h"
#include "Scene.h"
#include "Graphics/ComputePipeline.h"
class ComputeShaderPipelines
{



public:
    VulkanBase* base;
    Scene* scene;
    VulkanTexture3D ct3D;
    VulkanTexture3D ct3D2;

    VulkanTexture3D ex3D;
    VulkanTexture3D ex3D2;
    std::vector<UBO> computeUBO;
    std::vector<UBO> computeUBO2;
    ComputePipeline* comp;
    ComputePipeline* comp2;
    int Frame = 0;
    ComputeShaderPipelines(VulkanBase* base, Scene* scene);
    void update(float time);



    VulkanTexture3D createComputeTexture3D(int w,int h,int d) {
        VulkanTexture3D texture;
        int width = w;
        int height = h;
        int depth = d;
        texture.width = width;
        texture.height = height;
        texture.depth = depth;
        texture.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        texture.imageType = VK_IMAGE_TYPE_3D;
        texture.imageViewType = VK_IMAGE_VIEW_TYPE_3D;

        const uint32_t memSize = width * height * depth * 4;

        VulkanImage image = VulkanImage(base->device, base->vulkandevice->physicalDevice);

        image.createImage(width, height, depth, VK_IMAGE_TYPE_3D,1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        base->vbuffer->transitionImageLayout(image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1);

        image.createImageView(VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, 1, VK_IMAGE_VIEW_TYPE_3D);

        texture.image = image.image;
        texture.imageView = image.imageView;
        texture.imageMemory = image.imageMemory;
        texture.imageSampler = image.createTextureSampler(1);//miplevels as parameter btw
        return texture;
    }
};

