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

    int size = 128;
    ct3D = createComputeTexture3D(size, size, size);
    ct3D2 = createComputeTexture3D(size, size, size);

    ex3D = createComputeTexture3D(size, size, size);
    ex3D2 = createComputeTexture3D(size, size, size);

    computeUBO = base->vbuffer->createUniformBuffers({ sizeof(computeUbo) });
    ComputeInput input;
    input.computeShaderPath = "shaders/sdfCompute.spv";
    input.descriptorTypes = { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
    comp = new ComputePipeline(base, input);
    comp->setupDescriptors(computeUBO, { ct3D,scene->textures[0],ex3D,ex3D2 });
    comp->dispatch(ct3D.width / 8, ct3D.height / 8, ct3D.depth / 8);



  
    computeUBO2 = base->vbuffer->createUniformBuffers({ sizeof(computeUbo) });
    input.computeShaderPath = "shaders/sdfCompute2.spv";
    input.descriptorTypes = { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
    comp2 = new ComputePipeline(base, input);
 //   comp2->setupDescriptors(computeUBO2, { ct3D2,ct3D,ex3D2,ex3D });
 //   comp2->dispatch(ct3D2.width / 8, ct3D2.height / 8, ct3D2.depth / 8);

}

void ComputeShaderPipelines::update(float time) {
    ImVec2 screen_pos = ImGui::GetMousePos();
    static ImVec2 mousex = screen_pos;

    glm::vec2 mosue_delta = glm::vec2(screen_pos.x - mousex.x, screen_pos.y - mousex.y);
    
    mousex = screen_pos;
    static glm::vec4 storedata(0.0, -0.2, 0., 0.);
    storedata.z = mosue_delta.x * 0.001 + storedata.z;
    storedata.y = mosue_delta.y * 0.002 + storedata.y;
    computeUbo cubo;
    cubo.time = time;
   // std::cout<<time<<"\n";
    cubo.data = glm::vec4(-0.0, 0., -0.3, -1.);
    cubo.data.z += storedata.z;
    cubo.data.y += storedata.y;
    if (ImGui::GetIO().KeyCtrl) {
        storedata.z += 0.001;

    }
    else if (ImGui::GetIO().KeyAlt) {
        storedata.z -= 0.001;

    }
    else if (ImGui::GetIO().KeyShift) {
        cubo.data.a = 1.5;
    }
    else if (ImGui::GetIO().KeySuper) {
        cubo.data.a = -1.5;
    }
    cubo.data.z = glm::sin(time)*0.25;
    cubo.data.x = glm::cos(time)*0.25;
    cubo.Frame = Frame;
    //  cubo.data.x -= storedata.z;
     //std::cout << cubo.data.a << "\n";
    computeUBO[0].map(base->device, sizeof(computeUbo), &cubo);
    comp->submit();
   // computeUBO2[0].map(base->device, sizeof(computeUbo), &cubo);
    //comp2->submit();
    Frame += 1;
    using namespace std::this_thread;     // sleep_for, sleep_until
    using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
    using std::chrono::system_clock;

    sleep_for(10ns);
  //  sleep_until(system_clock::now() + 1000000000ns);

}