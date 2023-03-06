


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



#include <openvr.h>

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
#include "MathUtils.h"
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 601;

const std::string MODEL_PATH = "models/cube.glb";

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
//uncomment to enable VR
//#define VR
#include "Scene.h"
#include "Loader.h"
#include "GUI/EditorGui.h"
#include <glm/gtx/matrix_decompose.hpp>


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

    Scene* scene;

    Loader* loader;

    VulkanImgui *vulkanImgui;

    EditorGui *editorGui;

    VkDescriptorPool descriptorPool;

    vr::IVRSystem *sym;
  //  Quad *quad;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
       // glfwWindowHint(GLFW_DECORATED, false);
     //   glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
        window = glfwCreateWindow(WIDTH, HEIGHT, "PlayGround Engine", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
         
    }

    static void dropCallback(GLFWwindow* window, int path_count, const char* paths[]) {

        auto app = reinterpret_cast<Vulkan*>(glfwGetWindowUserPointer(window));

        vkDeviceWaitIdle(app->base->device);
      
     //   for (int i = 0; i < path_count; i++) {
       //     const char* path = paths[i];
       //     app->loadModel(path);
     //   }
        app->loader->load(path_count, paths);
        app->scene->cleanDescriptors();
        app->scene->createSceneDescriptor();
        app->createCommandBuffers();
       // app->modelTraverse(app->scene->rootNode, glm::mat4(1.0));

    }

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        auto app = reinterpret_cast<Vulkan*>(glfwGetWindowUserPointer(window));
        app->scene->offset -= float(yoffset)*0.1;
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Vulkan*>(glfwGetWindowUserPointer(window));
        app->base->swapChain->framebufferResized = true;
    }

 
    void initVulkan() {

#ifdef VR
        vr::EVRInitError eError = vr::VRInitError_None;
        sym = vr::VR_Init(&eError, vr::VRApplication_Scene);
        vr::Eye_Left;
#endif

        base = new VulkanBase();
        base->init(window);
        scene = new Scene(base);
        loader = new Loader(base, scene);
        editorGui = new EditorGui(scene,base);
       // VulkanTexture ct = createComputeTexture();
        scene->textures.push_back(createTexture(TEXTURE_PATH.c_str()));
        scene->textures.push_back(createTexture(TEXTURE_PATH2.c_str()));
       // scene->textures.push_back(createTexture3D());
        scene->init();

        vulkanImgui = new VulkanImgui(base, window);
        const char* p[] = { MODEL_PATH.c_str() };
      //  loadModel(MODEL_PATH.c_str());
        loader->load(1,p);
     //   scene->materials[0].skinnedPipeline = scene->pipelines[1];
    //    scene->materials[0].type = TexturedMaterial;
       // MaterialTexturedData dat;
      //  dat.DiffuseTexture = 0;
       // scene->materials[0].materialData = &dat;

        //scene->meshes[0].texID = 0;


        //edit object 0 for 3D texture raymarching,you can comment while doing other stuffs
        /*
        scene->objects[0].shader[0].graphicsPipeline = scene->pipelines[4];
        Material material;
        material.type = TexturedMaterial;
        MaterialTexturedData* matData = new MaterialTexturedData();
        matData->DiffuseTexture = 2;
        material.materialData = matData;
        scene->materials.push_back(material);
        scene->meshes[scene->objects[0].meshID[0]].matID = scene->materials.size()-1;
        */




        //scene->meshes[scene->objects[0].meshID[0]].matID =  
        scene->createSceneDescriptor();
        createCommandBuffers();
        vulkanImgui->init();
        glfwSetScrollCallback(window, scroll_callback);
        
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            drawFrame();
        }
        
        vkDeviceWaitIdle(base->device);
    }
    
    void cleanDescriptors() {
        vkDestroyDescriptorPool(base->device, descriptorPool, nullptr);
   }
    void cleanCommandBuffers() {
        vkFreeCommandBuffers(base->device, base->vulkandevice->commandPool, static_cast<uint32_t>(base->commandBuffers.size()), base->commandBuffers.data());
    }

    void cleanupSwapChain() {
      
        base->swapChain->cleanSwapChain();
      
       // base->offscreenbuffer->clean();
        //scene->framebuffers[0]->clean();
        vulkanImgui->cleanFrameBuffers();
        cleanCommandBuffers();


        vkDestroySwapchainKHR(base->device, base->swapChain->swapChain, nullptr);


    }

    void cleanup() {

        vr::VR_Shutdown();
        scene->clean();
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
        scene->recreate();
   
        createCommandBuffers();

        
    }

    VulkanTexture createTexture(const char* path) {
        VulkanTexture texture;
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
    VulkanTexture createComputeTexture() {
        VulkanTexture texture;
        int width = 512;
        int height = 512;
        texture.width = width;
        texture.height = height;
        VulkanImage image =  VulkanImage(base->device,base->vulkandevice->physicalDevice);

        image.createImage(width, height, 1, VK_IMAGE_TYPE_2D, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        image.createImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 1, VK_IMAGE_VIEW_TYPE_2D);

        texture.image = image.image;
        texture.imageView = image.imageView;
        texture.imageMemory = image.imageMemory;
        return texture;
    }
    VulkanTexture createTexture3D() {
        VulkanTexture3D texture;
        int width, height, depth;
        width = height = depth = 512;
        const uint32_t memSize = width * height * depth * 4;
        uint8_t* data = new uint8_t[memSize];
        int32_t index = 0;
        for (int32_t z = 0; z < depth; z++)
        {
            for (int32_t y = 0; y < height; y++)
            {
                for (int32_t x = 0; x < width; x++)
                {
                    /*
                    glm::vec3 p = glm::vec3(float(x) / float(width), float(y) / float(height), float(z) / float(depth));
                    
                    p -= glm::vec3(0.5);
            
                    float dis = glm::length(p)-0.2;
                 uint8_t dat = static_cast<uint8_t>((dis * 255));*/
                   uint8_t dat = 255;
                    data[index + 0] = dat;
                    data[index + 1] = dat;
                    data[index + 2] = dat;
                    data[index + 3] = 255;
                    index+=4;
                }
            }
        }
        float n = memSize;
        VkDeviceSize imageSize = width * height * depth * 4;
        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
        texture.width = width;
        texture.height = height;
        texture.depth = depth;
        texture.mipLevels = 1;
        base->vbuffer->createTexture3DfromBuffer(texture, data);
        free(data);
        return texture;
    }
    
   
    void createCommandBuffers() {

        scene->renderPass();
    }

   
    void updateUniformBuffer(uint32_t currentImage) {
       

    }
   
    static glm::mat4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t& matPose)
    {
        glm::mat4 matrixObj(
            matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
            matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
            matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
           matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
        );
        return matrixObj;
    }
   
    void VRsetup() {
        vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];

        vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);



        /////////////////UNCOMMENT FOR VR stuff TEST///////////////////////////
        vr::HmdMatrix44_t mat = sym->GetProjectionMatrix(vr::Eye_Left, 0.001, 10.);


        //   vr::HmdMatrix44_t mat;
        glm::mat4 proj[2];
        proj[0] = glm::mat4(mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
            mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
            mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
            mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]);
        /////////////////UNCOMMENT FOR VR stuff TEST///////////////////////////
        mat = sym->GetProjectionMatrix(vr::Eye_Right, 0.001, 10.);

        proj[1] = glm::mat4(mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
            mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
            mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
            mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]);

        int m_iValidPoseCount = 0;
        std::string m_strPoseClasses = "";
        glm::mat4 m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];
        char m_rDevClassChar[vr::k_unMaxTrackedDeviceCount];
        glm::mat4 viewpos;
        for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
        {
            if (m_rTrackedDevicePose[nDevice].bPoseIsValid)
            {
                m_iValidPoseCount++;
                m_rmat4DevicePose[nDevice] = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
                if (m_rDevClassChar[nDevice] == 0)
                {
                    switch (sym->GetTrackedDeviceClass(nDevice))
                    {
                    case vr::TrackedDeviceClass_Controller:        m_rDevClassChar[nDevice] = 'C'; break;
                    case vr::TrackedDeviceClass_HMD:               m_rDevClassChar[nDevice] = 'H'; break;
                    case vr::TrackedDeviceClass_Invalid:           m_rDevClassChar[nDevice] = 'I'; break;
                    case vr::TrackedDeviceClass_GenericTracker:    m_rDevClassChar[nDevice] = 'G'; break;
                    case vr::TrackedDeviceClass_TrackingReference: m_rDevClassChar[nDevice] = 'T'; break;
                    default:                                       m_rDevClassChar[nDevice] = '?'; break;
                    }
                }
                m_strPoseClasses += m_rDevClassChar[nDevice];
            }
        }

        if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
        {
            viewpos = m_rmat4DevicePose[vr::k_unTrackedDeviceIndex_Hmd];
            glm::mat4 pos = m_rmat4DevicePose[vr::k_unTrackedDeviceIndex_Hmd];
            pos = glm::rotate(pos, -3.1415f / 2.f, glm::vec3(1, 0, 0));

            viewpos = glm::inverse(viewpos);
            // viewpos = glm::translate(viewpos, glm::vec3(0, 0, -3));

            glm::vec3 position, scale, skew;
            glm::quat rot;
            glm::vec4 pers;
            glm::mat4 k = glm::inverse(viewpos);

            glm::decompose(viewpos, scale, rot, position, skew, pers);

            //  std::cout << position.x << " " << position.y << " " << position.z << "\n";
             // scene->objects[0].finalMatrix = glm::translate(glm::mat4(1.),glm::vec3(position.x,position.y, position.z));

        }
        if (m_rTrackedDevicePose[2].bPoseIsValid) {

            glm::mat4 con = m_rmat4DevicePose[2];
            con = glm::inverse(con);
            con = glm::rotate(con, -3.1415f / 2.f, glm::vec3(1, 0, 0));
            con = glm::inverse(con);
            con = glm::rotate(con, -3.1415f, glm::vec3(1, 0, 0));
            glm::vec3 position, scale, skew;
            glm::quat rot;
            glm::vec4 pers;
            // std::cout<<con
            glm::mat4 mat(1.0);
            mat = glm::translate(mat, glm::vec3(0., 0.2, -4.));
            //mat = glm::translate(mat, glm::vec3(position.x,position.z,position.y));
            //mat = glm::inverse(viewpos);
          //  for (unsigned int j = 0; j < scene->objects.size(); j++) {
           //     if (scene->objects[j].hasMesh) {
            //        scene->objects[j].finalMatrix = con * glm::scale( scene->objects[0].ModelMatrix,glm::vec3(0.2,0.2,0.2));
             //   }
           // }
           // scene->objects[1].finalMatrix = con * glm::scale(scene->objects[0].ModelMatrix, glm::vec3(0.2, 0.2, 0.2));
            for (Object& obj : scene->objects) {


                if (obj.hasMesh) {

                    if (scene->meshes[obj.meshID[0]].bones.size() > 1) {
                        obj.finalMatrix = con * obj.ModelMatrix;
                        obj.finalMatrix = glm::rotate(obj.finalMatrix, -3.1415f / 1.f, glm::vec3(1, 0, 0));
                        obj.finalMatrix = glm::scale(obj.finalMatrix, glm::vec3(0.0025, 0.0025, 0.0025));
                    }
                }
            }
            // scene->objects[0].finalMatrix = con *scene->objects[0].ModelMatrix;
             // scene->objects[0].finalMatrix = glm::mat4(0.0);

        }


        glm::mat4 view[2];

        ///////////vr stuff/////////////
        vr::HmdMatrix34_t matEyeRight = sym->GetEyeToHeadTransform(vr::Eye_Left);
        //vr::HmdMatrix34_t matEyeRight;
        view[0] = glm::mat4(matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
            matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
            matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
            matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f);

        glm::mat4 seperation = glm::mat4(1.0);
        seperation == glm::translate(seperation, glm::vec3(0.05, 0., 0.));

        view[0] = glm::inverse(view[0]);

        view[0] = view[0] * viewpos;
        view[0] = glm::rotate(view[0], -3.1415f / 2.f, glm::vec3(1, 0, 0));
        // view[0] 

         ///////vr stuff////
        matEyeRight = sym->GetEyeToHeadTransform(vr::Eye_Right);

        view[1] = glm::mat4(matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
            matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
            matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
            matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f);
        seperation = glm::mat4(1.0);
        seperation == glm::translate(seperation, glm::vec3(-0.05, 0., 0.));
        view[1] = glm::inverse(view[1]);
        view[1] = view[1] * viewpos;
        view[1] = glm::rotate(view[1], -3.1415f / 2.f, glm::vec3(1, 0, 0));
        //glm::translate(seperation, glm::vec3(-0.05, 0., 0.));

        glm::mat4 transformation; // your transformation matrix.
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;





        scene->proj[0] = proj[0];
        scene->view[0] = view[0];

        scene->proj[1] = proj[1];
        scene->view[1] = view[1];
    }

    void renderVR() {
        vr::VRVulkanTextureArrayData_t vData;

        vData.m_nImage = (uint64_t)scene->framebuffers[0]->MultisampledColorImage->image;
        vData.m_pDevice = (VkDevice_T*)base->device;
        vData.m_pPhysicalDevice = (VkPhysicalDevice_T*)base->vulkandevice->physicalDevice;
        vData.m_pInstance = (VkInstance_T*)base->vkinstance;
        vData.m_pQueue = (VkQueue_T*)base->queue;
        vData.m_nQueueFamilyIndex = base->vulkandevice->queueID;
        vData.m_nWidth = scene->viewport_resolution.x;
        vData.m_nHeight = scene->viewport_resolution.y;
        vData.m_nFormat = base->swapChain->swapChainImageFormat;
        vData.m_nSampleCount = 1;
        vData.m_unArraySize = 2;
        vData.m_unArrayIndex = 0;

        /*
        vr::VRVulkanTextureData_t vulkanData;
        vulkanData.m_nImage = (uint64_t)scene->framebuffers[0]->MultisampledColorImage->image;
        vulkanData.m_pDevice = (VkDevice_T*)base->device;
        vulkanData.m_pPhysicalDevice = (VkPhysicalDevice_T*)base->vulkandevice->physicalDevice;
        vulkanData.m_pInstance = (VkInstance_T*)base->vkinstance;
        vulkanData.m_pQueue = (VkQueue_T*)base->queue;
        vulkanData.m_nQueueFamilyIndex = base->vulkandevice->queueID;
        vulkanData.m_nWidth = scene->viewport_resolution.x;
        vulkanData.m_nHeight = scene->viewport_resolution.y;
        vulkanData.m_nFormat = base->swapChain->swapChainImageFormat;
        vulkanData.m_nSampleCount = 1;

        */
        vr::Texture_t texture = { &vData, vr::TextureType_Vulkan, vr::ColorSpace_Auto };
        vr::EVRSubmitFlags::Submit_GlArrayTexture;
        vr::VRTextureBounds_t bounds;
        bounds.uMin = 0.0f;
        bounds.uMax = 1.0f;
        bounds.vMin = 0.0f;
        bounds.vMax = 1.0f;
        vr::EVRCompositorError cErr;

        //vr stuff
        cErr = vr::VRCompositor()->Submit(vr::Eye_Left, &texture, &bounds, vr::Submit_VulkanTextureWithArrayData);




        vr::VRVulkanTextureArrayData_t vData_right;

        vData_right.m_nImage = (uint64_t)scene->framebuffers[0]->MultisampledColorImage->image;
        vData_right.m_pDevice = (VkDevice_T*)base->device;
        vData_right.m_pPhysicalDevice = (VkPhysicalDevice_T*)base->vulkandevice->physicalDevice;
        vData_right.m_pInstance = (VkInstance_T*)base->vkinstance;
        vData_right.m_pQueue = (VkQueue_T*)base->queue;
        vData_right.m_nQueueFamilyIndex = base->vulkandevice->queueID;
        vData_right.m_nWidth = scene->viewport_resolution.x;
        vData_right.m_nHeight = scene->viewport_resolution.y;
        vData_right.m_nFormat = base->swapChain->swapChainImageFormat;
        vData_right.m_nSampleCount = 1;
        vData_right.m_unArraySize = 2;
        vData_right.m_unArrayIndex = 1;

        vr::Texture_t  texture_right = { &vData_right, vr::TextureType_Vulkan, vr::ColorSpace_Auto };
        vr::EVRSubmitFlags::Submit_GlArrayTexture;
        // vr::VRTextureBounds_t bounds;
        // bounds.uMin = 0.0f;
        // bounds.uMax = 1.0f;
         //bounds.vMin = 0.0f;
        // bounds.vMax = 1.0f;
        // 
        // 
 //         vr STUFF
      //   vr::EVRCompositorError cErr;

        cErr = vr::VRCompositor()->Submit(vr::Eye_Right, &texture_right, &bounds, vr::Submit_VulkanTextureWithArrayData);

        vr::VRCompositor()->PostPresentHandoff();

    }
    void drawFrame() {
        static int frameid = 0;
      //  if (frameid == 0) {
        static float time = 0.;

        time += 0.1;
            
        vulkanImgui->newFrame();
        editorGui->run();
        vr::VREvent_t event;
        //sym->PollNextEvent(&event, sizeof(event));

        uint32_t imageIndex = base->getFrameIndex();

        //IMGUI/////////////

        vulkanImgui->draw(imageIndex);

#ifdef VR
        VRsetup();
#endif
        scene->update();


        updateUniformBuffer(0);

        base->submitCommandBuffers.push_back(vulkanImgui->imguiCommandBuffer[imageIndex]);

      
        VkResult result;
        base->presentQueue(result);
#ifdef VR

        renderVR();
#endif       
        
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