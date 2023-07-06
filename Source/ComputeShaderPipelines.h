#pragma once
#include "Graphics/VulkanBase.h"
#include "Scene/Scene.h"
#include "Graphics/ComputePipeline.h"
#include "ComputeShaderPipelines/GaussianFluidSim.h"
#include "ComputeShaderPipelines/MeshToSdf.h"
class ComputeShaderPipelines
{


public:
    VulkanBase* base;
    Scene* scene;
   // VulkanTexture ct3D;
  //  VulkanTexture3D ct3D2;

    //VulkanTexture3D ex3D;
   // VulkanTexture3D ex3D2;
    std::vector<UBO> computeUBO;
    std::vector<UBO> computeUBO2;
    ComputePipeline* comp;
    ComputePipeline* comp2;
    int Frame = 0;

    GaussianFluidSim *fluidSim;
    MeshToSdf *meshtosdf;
    ComputeShaderPipelines(VulkanBase* base, Scene* scene);
    ~ComputeShaderPipelines();
    void update(float time);



   
};

