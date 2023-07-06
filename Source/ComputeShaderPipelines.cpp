#include "ComputeShaderPipelines.h"
#include "Imgui/imgui.h"
#include <chrono>
#include <thread>

struct computeUbo {
    glm::vec4 data;
    float time;
    int Frame;

};


ComputeShaderPipelines::ComputeShaderPipelines(VulkanBase* base,Scene *scene) {
    this->base = base;
    this->scene = scene;
    meshtosdf = new MeshToSdf(base, scene);
    fluidSim = new GaussianFluidSim(base,scene, meshtosdf->computeTexture); 
  //  ct3D = fluidSim->computeTexture;
    scene->ptextures.push_back(fluidSim->computeTexture);
}
ComputeShaderPipelines::~ComputeShaderPipelines() {
    delete meshtosdf;
    delete fluidSim;
}

void ComputeShaderPipelines::update(float time) {
    meshtosdf->update(time);
    fluidSim->update(time);
}