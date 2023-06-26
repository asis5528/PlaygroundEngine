#include "Scene.h"
#include <chrono>
#include <iostream>
#include "../Graphics/VulkanUtils.h"

Scene::Scene(VulkanBase* vbase) {
	this->base = vbase;
   
}

void Scene::init()
{
    standardSampler = base->vbuffer->createTextureSampler(1);
    
   
    Framebuffer* frameBuffer = new Framebuffer(base->vulkandevice, base->swapChain->swapChainExtent.width, base->swapChain->swapChainExtent.height, base->vulkandevice->getMaxUsableSampleCount(), base->swapChain->swapChainImageFormat, utils::findDepthFormat(base->vulkandevice->physicalDevice),2);
    framebuffers.push_back(frameBuffer);
    /*
    Framebuffer* frameBuffer_quad = new Framebuffer(base->vulkandevice, base->swapChain->swapChainExtent.width, base->swapChain->swapChainExtent.height, base->vulkandevice->getMaxUsableSampleCount(), base->swapChain->swapChainImageFormat, utils::findDepthFormat(base->vulkandevice->physicalDevice));
    framebuffers.push_back(frameBuffer_quad);

    Quad* quad = new Quad(base,framebuffers[1]);
    quad->fr2 = &frameBuffer[0];
    quad->createGraphicsPipeline("shaders/quadvert.spv", "shaders/quadfrag.spv");
    VulkanTexture quadtex;
    // quadtex.imageView = base->offscreenbuffer->MultisampledColorImage->imageView;
    quadtex.imageView = framebuffers[0]->MultisampledColorImage->imageView;
    quadtex.imageSampler = base->vbuffer->createTextureSampler(1);
    quadtex.imageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    quad->textures.push_back(quadtex);
    quad->createdescriptors();
    quads.push_back(quad);
    */
    postprocessingManager = new PostProcessingManager(base,framebuffers[0]);
    scenePipelines = new ScenePipelines(base, framebuffers[0]->renderPass);

    /*
    GraphicsPipeline* opipe = new GraphicsPipeline();
    pipelines.push_back(opipe);
    PipelineInput input;
    input.vertexShaderpath = "shaders/vert.spv";
    input.fragmentShaderpath = "shaders/frag.spv";
    input.renderPass = framebuffers[0]->renderPass;
 //   input.MultiviewLayer = frameBuffer[0].multiLayerView;
    input.device = base->device;
    input.msaaSamples = base->msaaSamples;
    input.viewportExtent = base->swapChain->swapChainExtent;
    input.descriptorTypes = { descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
        descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
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


    GraphicsPipeline* basic = new GraphicsPipeline();
    pipelines.push_back(basic);
    input.skinned = false;
    input.vertexShaderpath = "shaders/vert.spv";
    input.fragmentShaderpath = "shaders/Basic.spv";

    input.descriptorTypes = { descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
       descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_FRAGMENT_BIT} };
    pipelines[2]->bufferSize = { sizeof(UniformBufferObject) ,sizeof(BasicUbo) };
    pipelines[2]->setupPipeline(input);
    pipelines[2]->createGraphicsPipeline();
    float k = 1.;
    
    GraphicsPipeline* skinnedBasic = new GraphicsPipeline();
    pipelines.push_back(skinnedBasic);
    input.skinned = true;
    input.vertexShaderpath = "shaders/skinned.spv";
    pipelines[3]->bufferSize = { sizeof(SkinnedUniformBufferObject),sizeof(BasicUbo) };
    pipelines[3]->setupPipeline(input);
    pipelines[3]->createGraphicsPipeline();
    
    GraphicsPipeline* volumetric = new GraphicsPipeline();
    pipelines.push_back(volumetric);
    input.skinned = false;
    input.vertexShaderpath = "shaders/volumetricvert.spv";
    input.fragmentShaderpath = "shaders/volumetric.spv";

    input.descriptorTypes = { descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
         descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_FRAGMENT_BIT},
         descriptorTypes{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT} };
    pipelines[4]->bufferSize = { sizeof(UniformBufferObject),sizeof(BasicUbo) };
    pipelines[4]->setupPipeline(input);
    pipelines[4]->createGraphicsPipeline();
    */
    pipelines = scenePipelines->pipelines;
}

void Scene::renderPass() {


    for (size_t i = 0; i < base->commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(base->commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        // Image memory barrier to make sure that compute shader writes are finished before sampling from the texture
        //dont need this if compute isnt rendering realtime,its only for syncing the work between compute and graphics
        /*
        VkImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageMemoryBarrier.image = computeTexture.image;
        imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0, 1 };
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(
            base->commandBuffers[i],
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &imageMemoryBarrier);
*/
        framebuffers[0]->beginRenderPass(base->commandBuffers[i]);
        



        for (unsigned int j = 0; j < objects.size(); j++) {
            if (objects[j].hasMesh) {
                for (int meshI = 0; meshI < objects[j].meshID.size(); meshI++) {
                    Mesh mesh = meshes[objects[j].meshID[meshI]];
                    vkCmdBindPipeline(base->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, objects[j].shader[meshI].graphicsPipeline->graphicsPipeline);
                    vkCmdBindDescriptorSets(base->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, objects[j].shader[meshI].graphicsPipeline->pipelineLayout, 0, 1, &objects[j].shader[meshI].descriptorSet, 0, nullptr);
                    mesh.draw(base->commandBuffers[i]);
                }


            }
        }

        vkCmdEndRenderPass(base->commandBuffers[i]);

        /*
        framebuffers[1]->beginRenderPass(base->commandBuffers[i]);
        quads[0]->draw(base->commandBuffers[i]);
        vkCmdEndRenderPass(base->commandBuffers[i]);
        */
        postprocessingManager->renderPass(base->commandBuffers[i]);


        base->swapChain->beginRenderPass(base->commandBuffers[i], i);
         //quads[0]->draw(base->commandBuffers[i]);
        vkCmdEndRenderPass(base->commandBuffers[i]);



        if (vkEndCommandBuffer(base->commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

   
}

void Scene::update(float iTime) {
   
    float time = iTime;
    
  
    for (Animation& animation : animations) {
        animation.boneTransform(time);
    }
#ifndef VR
    ///for non vr////////
    
    proj[0] = glm::perspective(glm::radians(45.0f), (float)viewport_resolution.x / (float)viewport_resolution.y, 0.1f, 1000.0f);
    // offset =1.5;
    
    ///for non vr////////
    float angle = sin(iTime/30.)*0.3;
    float an = sin(iTime / 12.) * 0.3;
   // angle = 1.4;
    glm::vec3 camPos = glm::vec3(glm::sin(angle) * offset, glm::cos(angle) * offset, glm::sin(an)*0.);
   view[0] = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
#endif
  //  glm::vec3 camPos;
    UniformBufferObject ubo{};
    glm::mat4 k;

    ubo.view[0] = view[0];
    ubo.proj[0] = proj[0];
    ubo.proj[0][1][1] *= -1;

    ubo.view[1] = view[1];
    ubo.proj[1] = proj[1];
    ubo.proj[1][1][1] *= -1;

    

    ubo1 u{};
    u.time1 = iTime;
    u.time2 = 0;
    u.time3 = 0;
  //  objects[0].finalMatrix = glm::mat4(1.);



    ///for non vr////////
    //objects[0].finalMatrix = glm::translate(objects[0].finalMatrix, glm::vec3(0., 0,2.));
    for (Object& object : objects) {

        if(object.hasMesh){
            if (object.hasAnimation) {

                for (int meshID = 0; meshID < object.meshID.size(); meshID++) {


                    Mesh* mesh = &meshes[object.meshID[meshID]];
                    float k = 1.;
                   
                    SkinnedUniformBufferObject subo{};
                    subo.view[0] = ubo.view[0];
                    subo.proj[0] = ubo.proj[0];

                    subo.view[1] = ubo.view[1];
                    subo.proj[1] = ubo.proj[1];
                    
                    subo.model = object.finalMatrix;
                    
                    Animation* animation = &animations[object.animationID];
                    unsigned int meshBoneIndex = 0;



                    for (int i = 0; i < mesh->boneInfo.size(); i++) {
                        subo.boneMatrices[i] = mesh->boneInfo[i].BoneOffset;
                    }
                    for (int i = 0; i < mesh->boneInfo.size(); i++) {
                        std::string boneName = mesh->boneInfo[i].name;
                        for (int j = 0; j < animation->processedMatrices.size(); j++) {
                            if (boneName == animation->processedMatrices[j].name) {
                                glm::mat4 finalMat = animation->processedMatrices[j].transformedBone * mesh->boneInfo[i].BoneOffset;
                                glm::mat4 f(1.0);
                                subo.boneMatrices[i] = finalMat;
                            }
                        }

                    }
                    
                  //  for (int i = 0; i < object.meshID.size(); i++) {
                   
                    subo.model = object.finalMatrix;
                    subo.model = glm::scale(subo.model, glm::vec3(3, 3, 3));
                        object.shader[meshID].ubos[0].map(base->device, sizeof(subo), &subo);
                        if (mesh->texID >= 0) {
                            object.shader[meshID].ubos[1].map(base->device, sizeof(u), &u);
                        }
                        else {
                            BasicUbo bubo{};
                            bubo.color = mesh->color;
                            object.shader[meshID].ubos[1].map(base->device, sizeof(bubo), &bubo);
                        }
                        
                   // }
                }
            }
            else {
                //for (int meshID = 0; meshID < object.meshID.size(); meshID++) {
            
                    ubo.model = object.finalMatrix;

                    for (int i = 0; i < object.meshID.size(); i++) {
                        Mesh* mesh = &meshes[object.meshID[i]];
                        object.shader[i].ubos[0].map(base->device, sizeof(ubo), &ubo);
                        if (mesh->texID >= 0) {
                            object.shader[i].ubos[1].map(base->device, sizeof(u), &u);
                        }
                        else {
                            BasicUbo bubo{};
                            // bubo.color = mesh->color;
                            bubo.color = glm::inverse(ubo.model)* glm::vec4(camPos, 1.);
                            bubo.color.a = iTime;
                            object.shader[i].ubos[1].map(base->device, sizeof(bubo), &bubo);
                        }
                    }
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
        else {
            objectWithMeshSize += object.meshID.size()-1;
        }
    }
    base->vulkandescriptor->createDescriptorPool(objectWithMeshSize,
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER },
        objectWithMeshSize + 10, &descriptorPool);

    //std::vector<VkDescriptorSetLayout> layouts(objects.size(), base->objectPipeline->descriptorSetLayout);
    std::vector<VkDescriptorSetLayout> layouts;
    for (int i = 0; i < objects.size(); i++) {
        if(objects[i].hasMesh){
            for (int meshID = 0; meshID < objects[i].meshID.size(); meshID++) {
                layouts.push_back(objects[i].shader[meshID].graphicsPipeline->descriptorSetLayout);
            }
            
        }
    }
    std::vector<VkDescriptorSet> descriptorSets = base->vulkandescriptor->createDescriptorSets(objectWithMeshSize, layouts, descriptorPool);
   
    int j = 0;
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i].hasMesh) {
            for (int meshID = 0; meshID < objects[i].meshID.size(); meshID++) {
                objects[i].shader[meshID].descriptorSet = descriptorSets[j];
                j += 1;
            }
        }
    }


    for (size_t i = 0; i < objects.size(); i++) {
        if (objects[i].hasMesh){
            Object* object = &objects[i];
            VulkanTexture tex;
         
          
           for (int meshID = 0; meshID < objects[i].meshID.size(); meshID++) {

             //  int texID = meshes[object->meshID[meshID]].texID;
        
               std::vector<VulkanTexture> descriptorsTextures{};
               Material mat = materials[meshes[object->meshID[meshID]].matID];
               if (mat.type == TexturedMaterial) {
                   MaterialTexturedData *dat = (struct MaterialTexturedData*)mat.materialData;
                  // MaterialTexturedData k = *dat;
                   descriptorsTextures.push_back(textures[dat->DiffuseTexture]);
               }
               base->vulkandescriptor->updateDescriptorSets(object->shader[meshID].descriptorSet, { object->shader[meshID].ubos }, descriptorsTextures);
           }
        }
    }
   rootNode.updateTransoformation(glm::mat4(1.0),objects);

}

void Scene::cleanUniforms()
{
    for (size_t i = 0; i < objects.size(); i++) {
        for (int meshID = 0; meshID < objects[i].meshID.size(); meshID++) {
            for (int j = 0; j < objects[i].shader[meshID].ubos.size(); j++)
            {
                vkDestroyBuffer(base->device, objects[i].shader[meshID].ubos[j].buffer, nullptr);
                vkFreeMemory(base->device, objects[i].shader[meshID].ubos[j].bufferMemory, nullptr);
            }
        }
    }

    cleanDescriptors();
}
void Scene::clean() {
    cleanUniforms();
    for (VulkanTexture& texture : textures) {
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
  /*
    for (GraphicsPipeline*& pipeline : pipelines) {
        delete pipeline;
    }
    */
    delete scenePipelines;
    /*
    for (Quad*& quad : quads) {
        delete quad->graphicsPipeline;
        delete quad;
    }
    */
    delete postprocessingManager;
    for (Framebuffer*& fb : framebuffers) {
        delete fb;
    }


    for (Material& mat : materials) {
        if (mat.materialData!=NULL) {
            delete mat.materialData;
        }
        
    }
    vkDestroySampler(base->device, standardSampler, nullptr);
}
void Scene::recreate()
{
   
    VkExtent2D ex;
    ex.width = viewport_resolution.x;
    ex.height = viewport_resolution.y;

    for (Framebuffer*& framebuffer : framebuffers) {

        framebuffer->clean();
        framebuffer->width = ex.width;
        framebuffer->height = ex.height;
        framebuffer->recreate();
    }
    /*
    framebuffers[0]->clean();
    framebuffers[0]->width = viewport_resolution.x;
    framebuffers[0]->height = viewport_resolution.y;
    framebuffers[0]->recreate();
    //framebuffers[0]->colorFormat = VK_FORMAT_B8G8R8A8_SRGB;


    framebuffers[1]->clean();
    framebuffers[1]->width = viewport_resolution.x;
    framebuffers[1]->height = viewport_resolution.y;
    framebuffers[1]->recreate();
    //framebuffers[1]->colorFormat = VK_FORMAT_B8G8R8A8_SRGB;
    */

    scenePipelines->recreate(framebuffers[0]->renderPass, ex);
    /*
    for (GraphicsPipeline* &pipeline : pipelines) {
        pipeline->clean();
        pipeline->recreatePipeline(ex, framebuffers[0]->renderPass);
    }
    */
    //uncomment this when needs actual updating the frame texture
   // quads[0]->textures[0].imageView = quads[0]->fr2->MultisampledColorImage->imageView;
    /*
    for (Quad* &quad : quads) {
        quad->textures[0].imageView = quad->fr2->MultisampledColorImage->imageView;
        quad->graphicsPipeline->clean();
        quad->recreateGraphicsPipeline(ex);
        quad->cleandescriptors();
        quad->createdescriptors();
    }
    */
    postprocessingManager->recreate(ex);
 

    cleanDescriptors();
    createSceneDescriptor();

}
void Scene::cleanDescriptors() {
    vkDestroyDescriptorPool(base->device, descriptorPool, nullptr);
}