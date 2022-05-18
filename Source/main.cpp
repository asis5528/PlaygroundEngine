


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
#include "Scene.h"
#include "Loader.h"
#include "GUI/EditorGui.h"


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
  //  Quad *quad;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
       // glfwWindowHint(GLFW_DECORATED, false);
     //   glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
        window = glfwCreateWindow(WIDTH, HEIGHT, "PlayGround Engine", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        glfwSetDropCallback(window, dropCallback);
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

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Vulkan*>(glfwGetWindowUserPointer(window));
        app->base->swapChain->framebufferResized = true;
    }

 
    void initVulkan() {
        base = new VulkanBase();
        base->init(window);
        scene = new Scene(base);
        loader = new Loader(base, scene);
        editorGui = new EditorGui(scene,base);

        scene->textures.push_back(createTexture(TEXTURE_PATH.c_str()));
        scene->textures.push_back(createTexture(TEXTURE_PATH2.c_str()));
        scene->init();

        vulkanImgui = new VulkanImgui(base, window);
        const char* p[] = { MODEL_PATH.c_str() };
      //  loadModel(MODEL_PATH.c_str());
        loader->load(1,p);
        
        scene->createSceneDescriptor();
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
    
   
    void createCommandBuffers() {

        scene->renderPass();
    }

   
    void updateUniformBuffer(uint32_t currentImage) {
       

    }
   
   
   
    void drawFrame() {
        static int frameid = 0;
      //  if (frameid == 0) {
       
            
        vulkanImgui->newFrame();
        editorGui->run();
   
        

        uint32_t imageIndex = base->getFrameIndex();

        //IMGUI/////////////

        vulkanImgui->draw(imageIndex);
        scene->update();
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