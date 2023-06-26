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
    int Frame = 0;
    GaussianFluidSim(VulkanBase* base, Scene* scene);
    void update(float time);
};

