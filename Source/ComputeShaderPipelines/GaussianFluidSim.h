#pragma once
#include "../Graphics/VulkanBase.h"
#include "../Scene/Scene.h"
#include "../Graphics/ComputePipeline.h"
class GaussianFluidSim
{

public:
    VulkanBase* base;
    Scene* scene;
    PGEResourceTypes::Texture3D computeTexture;
    std::vector<UBO> computeUBO;
    ComputePipeline* comp;

    PGEResourceTypes::Texture3D computeTexture2;
    std::vector<UBO> computeUBO2;
    ComputePipeline* comp2;

    int Frame = 0;
    GaussianFluidSim(VulkanBase* base, Scene* scene, PGEResourceTypes::Texture3D sdftex);
    ~GaussianFluidSim();
    void update(float time);
};

