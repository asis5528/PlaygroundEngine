#pragma once
#include "../Graphics/VulkanBase.h"
#include "../Scene/Scene.h"
#include "../Graphics/ComputePipeline.h"
class GaussianFluidSim
{

public:
    VulkanBase* base;
    Scene* scene;
    VulkanTexture3D computeTexture;
    std::vector<UBO> computeUBO;
    ComputePipeline* comp;

    VulkanTexture3D computeTexture2;
    std::vector<UBO> computeUBO2;
    ComputePipeline* comp2;

    int Frame = 0;
    GaussianFluidSim(VulkanBase* base, Scene* scene, VulkanTexture3D sdftex);
    void update(float time);
};

