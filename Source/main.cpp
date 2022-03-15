


#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <optional>
#include <set>
#include <unordered_map>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::string MODEL_PATH = "models/spiderman.obj";

const std::string TEXTURE_PATH = "textures/spider.png";
const std::string TEXTURE_PATH2 = "textures/viking_room.png";
const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,VK_KHR_MULTIVIEW_EXTENSION_NAME,VK_KHR_MAINTENANCE2_EXTENSION_NAME,VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
#include "Graphics/Mesh.h"
#include"Graphics/VulkanBase.h"
#include "Graphics/IMGUI/VulkanImgui.h"
#include "Object.h"
#include "quad.h"


namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}





inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from)
{
    glm::mat4 to;


    to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
    to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
    to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
    to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

    return to;
}
class Vulkan {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;

    VulkanBase *base;

    GraphicsPipeline* objectPipeline;

    std::vector<Texture> textures;
    std::vector<Mesh> meshes;
     std::vector<Object> objects;
   
    VulkanImgui *vulkanImgui;

    VkDescriptorPool descriptorPool;
    Quad *quad;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        glfwSetDropCallback(window, dropCallback);
    }

    static void dropCallback(GLFWwindow* window, int path_count, const char* paths[]) {

        auto app = reinterpret_cast<Vulkan*>(glfwGetWindowUserPointer(window));

        vkDeviceWaitIdle(app->base->device);
      
        for (int i = 0; i < path_count; i++) {
            const char* path = paths[i];
            app->loadModel(path);
        }
       
        app->cleanDescriptors();
        app->createDescriptors();
        app->createCommandBuffers();

    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Vulkan*>(glfwGetWindowUserPointer(window));
        app->base->swapChain->framebufferResized = true;
    }

 
    void initVulkan() {
        base = new VulkanBase();
        base->init(window);

        textures.push_back(createTexture(TEXTURE_PATH.c_str()));
        textures.push_back(createTexture(TEXTURE_PATH2.c_str()));

        quad = new Quad(base);
        quad->createGraphicsPipeline("shaders/quadvert.spv", "shaders/quadfrag.spv");
        Texture quadtex;
        quadtex.imageView = base->offscreenbuffer->MultisampledColorImage->imageView;
        quadtex.imageSampler = textures[0].imageSampler;

        quad->textures.push_back(quadtex);
        quad->createdescriptors();

        objectPipeline = new GraphicsPipeline();
        PipelineInput input;
        input.vertexShaderpath = "shaders/vert.spv";
        input.fragmentShaderpath = "shaders/frag.spv";
        input.renderPass = base->offscreenbuffer->renderPass;
        input.device = base->device;
        input.msaaSamples = base->msaaSamples;
        input.viewportExtent = base->swapChain->swapChainExtent;
        input.descriptorTypes = { descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
            descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_FRAGMENT_BIT},
            descriptorTypes{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT} };
        objectPipeline->bufferSize = { sizeof(UniformBufferObject),sizeof(ubo1) };
        objectPipeline->setupPipeline(input);
        objectPipeline->createGraphicsPipeline();

       
        vulkanImgui = new VulkanImgui(base, window);
        
        loadModel(MODEL_PATH.c_str());
        createDescriptors();
        createCommandBuffers();
        vulkanImgui->init();
        
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            drawFrame();
        }
        
        vkDeviceWaitIdle(base->device);
    }

    void cleanUniforms() {
        for (size_t i = 0; i < objects.size(); i++) {
            for (int j = 0; j < objects[i].shader.ubos.size(); j++)
            {
            vkDestroyBuffer(base->device, objects[i].shader.ubos[j].buffer, nullptr);
            vkFreeMemory(base->device, objects[i].shader.ubos[j].bufferMemory, nullptr);
            }
        }

        vkDestroyDescriptorPool(base->device, descriptorPool, nullptr);
    }

    void cleanDescriptors() {
        vkDestroyDescriptorPool(base->device, descriptorPool, nullptr);
   }
    void cleanCommandBuffers() {
        vkFreeCommandBuffers(base->device, base->vulkandevice->commandPool, static_cast<uint32_t>(base->commandBuffers.size()), base->commandBuffers.data());
    }

    void cleanupSwapChain() {
     
        for (auto framebuffer : base->swapChain->swapChainFramebuffers) {
            vkDestroyFramebuffer(base->device, framebuffer, nullptr);
        }
        cleanCommandBuffers();


        vkDestroyPipeline(base->device, objectPipeline->graphicsPipeline, nullptr);
        
        vkDestroyRenderPass(base->device, base->swapChain->renderPass, nullptr);

        for (auto imageView : base->swapChain->swapChainImageViews) {
            vkDestroyImageView(base->device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(base->device, base->swapChain->swapChain, nullptr);


    }

    void cleanup() {

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
        
        
        delete objectPipeline;
        delete quad->graphicsPipeline;
        delete quad;
        delete vulkanImgui;
        delete base;
        
        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void recreateSwapChain() {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(base->device);

        cleanupSwapChain();
        //base->recreate();
       
        base->recreate();
      
        vulkanImgui->recreate();
      
        objectPipeline->recreatePipeline(base->swapChain->swapChainExtent,base->offscreenbuffer->renderPass);
        quad->graphicsPipeline->recreatePipeline(base->swapChain->swapChainExtent,base->swapChain->renderPass);
      

        quad->textures[0].imageView = base->offscreenbuffer->MultisampledColorImage->imageView;

        quad->cleandescriptors();
        quad->createdescriptors();


        cleanDescriptors();
        createDescriptors();
        createCommandBuffers();

        
    }

    Texture createTexture(const char* path) {
        Texture texture;
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
        texture.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
        texture.width = texWidth;
        texture.height = texHeight;
        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }
        base->vbuffer->createTexturefromBuffer(texture, pixels);
        stbi_image_free(pixels);
        return texture;

    }
    

    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            const char* stt = node->mName.C_Str();
            const char* strr = "defaultobject";
            const char* strk = "defaultobject";
            if (std::string(node->mName.C_Str()) == std::string(strr)) {
                float k = 0.0;
            }
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            Mesh meshbuffer;
            processMesh(mesh, scene, meshbuffer);
            base->vbuffer->createVertexBuffer(meshbuffer);
            base->vbuffer->createIndexBuffer(meshbuffer);
           
            meshes.push_back(meshbuffer);
            Object object;
            object.meshID = meshes.size() - 1;
            object.textureID = 0;
            const aiMatrix4x4 mat = node->mTransformation;
            object.ModelMatrix = aiMatrix4x4ToGlm(&mat);
            int sizee = (objects.size()) + ((objects.size() + 1) % 2);
            glm::mat4 transl = glm::translate(glm::mat4(1.0), glm::vec3(sizee / 3., 0.0, 0.0));
            object.ModelMatrix = transl * object.ModelMatrix;
            Shader shader;
            shader.graphicsPipeline = objectPipeline;
            object.shader = shader;
            object.shader.ubos = base->vbuffer->createUniformBuffers(object.shader.graphicsPipeline->bufferSize);
        
            objects.push_back(object);
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            if (std::string(node->mChildren[i]->mName.C_Str()) != std::string("defaultobject")) {
                processNode(node->mChildren[i], scene);
            }
        }

    }
    void processMesh(aiMesh* mesh, const aiScene* scene, Mesh& meshbuffer)
    {
        //data to fill
        //vector<Vertex> vertices;
        //vector<unsigned int> indices;
        //vector<Texture> textures;
        //Mesh meshbuffer;
        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.pos = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoord = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                // vertex.Tangent = vector;
                 // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                // vertex.Bitangent = vector;
            }
            else
                vertex.texCoord = glm::vec2(0.0f, 0.0f);

            meshbuffer.vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                meshbuffer.indices.push_back(face.mIndices[j]);
        }
        // return meshbuffer;

    }



    void loadModel(const char* MODEL_PATH) {


        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(MODEL_PATH, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        processNode(scene->mRootNode, scene);



    }
   
    void createDescriptors() {

        base->vulkandescriptor->createDescriptorPool(objects.size(),
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER },
            objects.size() + 10, &descriptorPool);

        //std::vector<VkDescriptorSetLayout> layouts(objects.size(), base->objectPipeline->descriptorSetLayout);
        std::vector<VkDescriptorSetLayout> layouts;
        for (int i = 0; i < objects.size(); i++) {
            layouts.push_back(objects[i].shader.graphicsPipeline->descriptorSetLayout);
        }
        std::vector<VkDescriptorSet> descriptorSets = base->vulkandescriptor->createDescriptorSets(objects.size(), layouts, descriptorPool);
        for (int i = 0; i < objects.size(); i++) {

            objects[i].shader.descriptorSet = descriptorSets[i];
        }
       

        for (size_t i = 0; i < objects.size(); i++) {
            Object *object = &objects[i];
            Texture tex;
            if (i == 0) {
                tex = textures[0];
            }
            else {
                tex = textures[1];
            }
            base->vulkandescriptor->updateDescriptorSets(object->shader.descriptorSet, {object->shader.ubos }, { tex });
        }

        /*
        std::vector<VkDescriptorSetLayout> Qlayouts(1, quad->graphicsPipeline->descriptorSetLayout);
        quadSet = base->vulkandescriptor->createDescriptorSets(1, Qlayouts, descriptorPool)[0];

        Texture quadtex;
        quadtex.imageView = base->offscreenbuffer->MultisampledColorImage->imageView;
        quadtex.imageSampler = textures[0].imageSampler;;
        base->vulkandescriptor->updateDescriptorSets(quadSet, {  }, { quadtex });

    */
    
    }
  
    void createCommandBuffers() {



        for (size_t i = 0; i < base->commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(base->commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }
            
            base->offscreenbuffer->beginRenderPass(base->commandBuffers[i]);

            vkCmdBindPipeline(base->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, objectPipeline->graphicsPipeline);


            for (unsigned int j = 0; j < objects.size(); j++) {

                Mesh mesh = meshes[objects[j].meshID];
             
                vkCmdBindDescriptorSets(base->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, objectPipeline->pipelineLayout, 0, 1, &objects[j].shader.descriptorSet, 0, nullptr);
                mesh.draw(base->commandBuffers[i]);
              

            }

            vkCmdEndRenderPass(base->commandBuffers[i]);

            
            base->swapChain->beginRenderPass(base->commandBuffers[i], i);
            // vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


            quad->draw(base->commandBuffers[i]);
            
            vkCmdEndRenderPass(base->commandBuffers[i]);



            if (vkEndCommandBuffer(base->commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

   
    void updateUniformBuffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};
        ubo.view = glm::lookAt(glm::vec3(glm::sin(time / 3.) * 6., glm::cos(time / 3.) * 6., 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), base->swapChain->swapChainExtent.width / (float)base->swapChain->swapChainExtent.height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;
        ubo1 u{};
        u.time1 = time;
        u.time2 = 0;
        u.time3 = 0;

        for (int i = 0; i < objects.size(); i++) {
            ubo.model = objects[i].ModelMatrix;

            objects[i].shader.ubos[0].map(&base->device, sizeof(ubo), &ubo);

          
            objects[i].shader.ubos[1].map(&base->device, sizeof(u), &u);
           
        }

    }

    void drawFrame() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();

        uint32_t imageIndex = base->getFrameIndex();

        //IMGUI/////////////

        vulkanImgui->draw(imageIndex);
        updateUniformBuffer(0);

        base->submitCommandBuffers.push_back(vulkanImgui->imguiCommandBuffer[imageIndex]);

        
        VkResult result;
        base->presentQueue(result);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || base->swapChain->framebufferResized) {
            base->swapChain->framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

    }
   
   
};



int main() {
    Vulkan app;
    system("compile.bat");
    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}