#include "Scene.h"
#include <chrono>
#include <iostream>
#include "Graphics/VulkanUtils.h"
Scene::Scene(VulkanBase* vbase) {
	this->base = vbase;
   
}

void Scene::init()
{

    Framebuffer* frameBuffer = new Framebuffer(base->vulkandevice, base->swapChain->swapChainExtent.width, base->swapChain->swapChainExtent.height, base->vulkandevice->getMaxUsableSampleCount(), base->swapChain->swapChainImageFormat, utils::findDepthFormat(base->vulkandevice->physicalDevice));
    framebuffers.push_back(frameBuffer);

    Quad* quad = new Quad(base);
    quad->createGraphicsPipeline("shaders/quadvert.spv", "shaders/quadfrag.spv");
    Texture quadtex;
   // quadtex.imageView = base->offscreenbuffer->MultisampledColorImage->imageView;
    quadtex.imageView = framebuffers[0]->MultisampledColorImage->imageView;
    quadtex.imageSampler = textures[0].imageSampler;

    quad->textures.push_back(quadtex);
    quad->createdescriptors();
    quads.push_back(quad);
    

   
    

    GraphicsPipeline* opipe = new GraphicsPipeline();
    pipelines.push_back(opipe);
    PipelineInput input;
    input.vertexShaderpath = "shaders/vert.spv";
    input.fragmentShaderpath = "shaders/frag.spv";
    input.renderPass = framebuffers[0]->renderPass;
    input.device = base->device;
    input.msaaSamples = base->msaaSamples;
    input.viewportExtent = base->swapChain->swapChainExtent;
    input.descriptorTypes = { descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
        descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_FRAGMENT_BIT},
        descriptorTypes{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT} };
    pipelines[0]->bufferSize = { sizeof(UniformBufferObject),sizeof(ubo1) };
    pipelines[0]->setupPipeline(input);
    pipelines[0]->createGraphicsPipeline();

    GraphicsPipeline* skinned = new GraphicsPipeline();
    pipelines.push_back(skinned);
    input.skinned = true;
    input.vertexShaderpath = "shaders/skinned.spv";
    pipelines[1]->bufferSize = { sizeof(SkinnedUniformBufferObject),sizeof(ubo1) };
    pipelines[1]->setupPipeline(input);
    pipelines[1]->createGraphicsPipeline();
}

void Scene::renderPass() {


    for (size_t i = 0; i < base->commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(base->commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        framebuffers[0]->beginRenderPass(base->commandBuffers[i]);




        for (unsigned int j = 0; j < objects.size(); j++) {
            if (objects[j].hasMesh) {
                for (int meshI = 0; meshI < objects[j].meshID.size(); meshI++) {
                    Mesh mesh = meshes[objects[j].meshID[meshI]];
                    vkCmdBindPipeline(base->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, objects[j].shader.graphicsPipeline->graphicsPipeline);
                    vkCmdBindDescriptorSets(base->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, objects[j].shader.graphicsPipeline->pipelineLayout, 0, 1, &objects[j].shader.descriptorSet, 0, nullptr);
                    mesh.draw(base->commandBuffers[i]);
                }


            }
        }

        vkCmdEndRenderPass(base->commandBuffers[i]);


        base->swapChain->beginRenderPass(base->commandBuffers[i], i);
        // scene->quads[0]->draw(base->commandBuffers[i]);
        vkCmdEndRenderPass(base->commandBuffers[i]);



        if (vkEndCommandBuffer(base->commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

   
}

void Scene::update() {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float iTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    float time = iTime;
  
    for (Animation& animation : animations) {
        animation.boneTransform(time);
    }
  
   

    UniformBufferObject ubo{};
    float offset = 16.;
    ubo.view = glm::lookAt(glm::vec3(glm::sin(iTime / 3.) * offset, glm::cos(iTime / 3.) * offset, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f),(float)viewport_resolution.x / (float)viewport_resolution.y, 0.1f, 1000.0f);
    ubo.proj[1][1] *= -1;

    
    

    ubo1 u{};
    u.time1 = iTime;
    u.time2 = 0;
    u.time3 = 0;
    int debL = 0;
    for (Object& object : objects) {

        if(object.hasMesh){
            if (object.hasAnimation) {

                Mesh* mesh = &meshes[object.meshID[0]];
                float k = 1.;
                for (BoneInfo& info : mesh->boneInfo) {
                    float k = 1.;
                }
                SkinnedUniformBufferObject subo{};
                subo.view = ubo.view;
                subo.proj = ubo.proj;
                subo.model = object.finalMatrix;

                Animation* animation = &animations[object.animationID];
                unsigned int meshBoneIndex = 0;
                /*
                unsigned int j = 0;
                for (unsigned int i = 0; i < animation->processedMatrices.size(); i++) {
                    if (animation->processedMatrices[i].name != mesh->boneInfo[j].name) {
                        //data->info.erase(data->info.begin() + i);


                    }
                    else {
                        subo.boneMatrices[j] = animation->processedMatrices[i].transformedBone* mesh->boneInfo[i].BoneOffset;
                        j += 1;
                        if (j > mesh->boneInfo.size() - 1) {
                            break;
                        }
                    }

                }*/
            

                for (int i = 0; i < mesh->boneInfo.size(); i++) {
                    subo.boneMatrices[i] = mesh->boneInfo[i].BoneOffset;
                }
                for (int i = 0; i < mesh->boneInfo.size(); i++) {
                    std::string boneName = mesh->boneInfo[i].name;
                   for (int j = 0; j < animation->processedMatrices.size(); j++) {
                        if (boneName == animation->processedMatrices[j].name) { 
                            glm::mat4 finalMat =   animation->processedMatrices[j].transformedBone* mesh->boneInfo[i].BoneOffset;
                            glm::mat4 f(1.0);
                            subo.boneMatrices[i] = finalMat;
                        }
                    }
                   // subo.boneMatrices[i] = mesh->boneInfo[i].BoneOffset;
                }
               // subo.boneMatrices[0][0].x = 1.;
              //  subo.boneMatrices[0][0].y = 0.;
              //  subo.boneMatrices[0][0].z = 1.;

                /*
                for (int i = 0; i < animation->processedMatrices.size(); i++) {
                    if (mesh->boneInfo[meshBoneIndex].name == animation->processedMatrices[i].name) {
                    
                   
                        glm::mat finalMat = mesh->boneInfo[meshBoneIndex].BoneOffset* animation->processedMatrices[i].transformedBone;
                        subo.boneMatrices[meshBoneIndex] = finalMat;
                    
                        meshBoneIndex++;
                        if (meshBoneIndex > mesh->boneInfo.size() - 1) break;
                    }
                    else {
                        float k = 1.;
                    }
            
                }*/
              //  std::fill_n(subo.boneMatrices, 99, glm::mat4(1.0));
                object.shader.ubos[0].map(&base->device, sizeof(subo), &subo);
                object.shader.ubos[1].map(&base->device, sizeof(u), &u);
            }
            else {
                ubo.model = object.finalMatrix;
                object.shader.ubos[0].map(&base->device, sizeof(ubo), &ubo);
                object.shader.ubos[1].map(&base->device, sizeof(u), &u);
            }
    }

    /*
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i].hasAnimation) {
            subo.model = objects[i].ModelMatrix;

            for (int i = 0; i < animations[objects[i].animationID].processedMatrices.size(); i++) {
            
            }

            objects[i].shader.ubos[0].map(&base->device, sizeof(subo), &subo);
            objects[i].shader.ubos[1].map(&base->device, sizeof(u), &u);
        }else{
        ubo.model = objects[i].ModelMatrix;
        objects[i].shader.ubos[0].map(&base->device, sizeof(ubo), &ubo);
        objects[i].shader.ubos[1].map(&base->device, sizeof(u), &u);
        }
    }*/
    }
}

void Scene::createSceneDescriptor()
{
    unsigned int objectWithMeshSize = objects.size();
    for (Object&object:objects) {
        if (!object.hasMesh) {
            objectWithMeshSize -=1;
        }
    }
    base->vulkandescriptor->createDescriptorPool(objectWithMeshSize,
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER },
        objectWithMeshSize + 10, &descriptorPool);

    //std::vector<VkDescriptorSetLayout> layouts(objects.size(), base->objectPipeline->descriptorSetLayout);
    std::vector<VkDescriptorSetLayout> layouts;
    for (int i = 0; i < objects.size(); i++) {
        if(objects[i].hasMesh)
        layouts.push_back(objects[i].shader.graphicsPipeline->descriptorSetLayout);
    }
    std::vector<VkDescriptorSet> descriptorSets = base->vulkandescriptor->createDescriptorSets(objectWithMeshSize, layouts, descriptorPool);
   
    int j = 0;
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i].hasMesh) {
            objects[i].shader.descriptorSet = descriptorSets[j];
            j += 1;
        }
    }


    for (size_t i = 0; i < objects.size(); i++) {
        if (objects[i].hasMesh){
            Object* object = &objects[i];
            Texture tex;
            if (i == 0) {
                tex = textures[0];
            }
            else {
                tex = textures[0];
            }
            base->vulkandescriptor->updateDescriptorSets(object->shader.descriptorSet, { object->shader.ubos }, { tex });
        }
    }
   rootNode.updateTransoformation(glm::mat4(1.0),objects);

}

void Scene::cleanUniforms()
{
    for (size_t i = 0; i < objects.size(); i++) {
        for (int j = 0; j < objects[i].shader.ubos.size(); j++)
        {
            vkDestroyBuffer(base->device, objects[i].shader.ubos[j].buffer, nullptr);
            vkFreeMemory(base->device, objects[i].shader.ubos[j].bufferMemory, nullptr);
        }
    }

    cleanDescriptors();
}
void Scene::clean() {
    cleanUniforms();
    for (Texture& texture : textures) {
        vkDestroyImageView(base->device, texture.imageView, nullptr);
        vkDestroyImage(base->device, texture.image, nullptr);
        vkFreeMemory(base->device, texture.imageMemory, nullptr);
        vkDestroySampler(base->device, texture.imageSampler, nullptr);
    }

    for (Mesh& mesh : meshes) {
        vkDestroyBuffer(base->device, mesh.indexBuffer, nullptr);
        vkFreeMemory(base->device, mesh.indexBufferMemory, nullptr);

        vkDestroyBuffer(base->device, mesh.vertexBuffer, nullptr);
        vkFreeMemory(base->device, mesh.vertexBufferMemory, nullptr);
    }
  
    for (GraphicsPipeline*& pipeline : pipelines) {
        delete pipeline;
    }
    for (Quad*& quad : quads) {
        delete quad->graphicsPipeline;
        delete quad;
    }
    for (Framebuffer*& fb : framebuffers) {
        delete fb;
    }

}
void Scene::recreate()
{
    framebuffers[0]->clean();
    framebuffers[0]->width = viewport_resolution.x;
    framebuffers[0]->height = viewport_resolution.y;
    framebuffers[0]->recreate();
    framebuffers[0]->colorFormat = VK_FORMAT_B8G8R8A8_SRGB;
    VkExtent2D ex;
    ex.width = viewport_resolution.x;
    ex.height = viewport_resolution.y;
    for (GraphicsPipeline* &pipeline : pipelines) {
        pipeline->clean();
        pipeline->recreatePipeline(ex, framebuffers[0]->renderPass);
    }
    for (Quad* &quad : quads) {
        quad->graphicsPipeline->clean();
        quad->graphicsPipeline->recreatePipeline(ex, base->swapChain->renderPass);
    }
    quads[0]->textures[0].imageView = framebuffers[0]->MultisampledColorImage->imageView;
    for (Quad*& quad : quads) {
        quad->cleandescriptors();
        quad->createdescriptors();
    }

    cleanDescriptors();
    createSceneDescriptor();

}
void Scene::cleanDescriptors() {
    vkDestroyDescriptorPool(base->device, descriptorPool, nullptr);
}