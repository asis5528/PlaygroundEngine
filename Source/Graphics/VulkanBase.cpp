#include "VulkanBase.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
#include <iostream>
#include <GLFW/glfw3.h>
#include "VulkanUtils.h"
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

void VulkanBase::init(GLFWwindow* window) {
    this->window = window;
    createInstance();

    setupSurface();
    pickPhysicalDevice();
   
    vulkandevice = new VulkanDevice(vkPhysicalDevice);
    VkPhysicalDeviceFeatures2KHR deviceFeatures2{};
    VkPhysicalDeviceMultiviewFeaturesKHR extFeatures{};
    extFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
    deviceFeatures2.pNext = &extFeatures;
    PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2KHR>(vkGetInstanceProcAddr(vkinstance, "vkGetPhysicalDeviceFeatures2KHR"));
    vkGetPhysicalDeviceFeatures2KHR(vkPhysicalDevice, &deviceFeatures2);
    std::cout << "Multiview features:" << std::endl;
    std::cout << "\tmultiview = " << extFeatures.multiview << std::endl;
    std::cout << "\tmultiviewGeometryShader = " << extFeatures.multiviewGeometryShader << std::endl;
    std::cout << "\tmultiviewTessellationShader = " << extFeatures.multiviewTessellationShader << std::endl;
    std::cout << std::endl;

    VkPhysicalDeviceProperties2KHR deviceProps2{};
    VkPhysicalDeviceMultiviewPropertiesKHR extProps{};
    extProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR;
    deviceProps2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
    deviceProps2.pNext = &extProps;
    PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(vkGetInstanceProcAddr(vkinstance, "vkGetPhysicalDeviceProperties2KHR"));
    vkGetPhysicalDeviceProperties2KHR(vkPhysicalDevice, &deviceProps2);

    vulkandevice->createLogicalDevice();
    vulkandevice->createCommandPool();
    
    VkBool32 R;
    vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, vulkandevice->queueID, vksurface,&R );
    if (!R) {
        throw std::runtime_error("surface doesn't support the device");
    }
    this->device = vulkandevice->device;
    this->queue = vulkandevice->Queue;
    msaaSamples = vulkandevice->getMaxUsableSampleCount();
    this->swapChain = new SwapChain(vkPhysicalDevice,device,vksurface, window,this->queue, msaaSamples);
    createCommandBuffers();


   // offscreenbuffer = new Framebuffer(vulkandevice,swapChain->swapChainExtent.width, swapChain->swapChainExtent.height,msaaSamples,swapChain->swapChainImageFormat, utils::findDepthFormat(vkPhysicalDevice));
   
    
  vbuffer = new  VBuffer(vulkandevice);
  vulkandescriptor = new VulkanDescriptor(vulkandevice);
  createSyncObjects();
    ////////////////////////////////////FIVRRRR//////////////////////////////////////////////
}

void VulkanBase::recreate() {
  //  swapChain->create();
    swapChain->create();
    
    createCommandBuffers();
  //  vkResetCommandPool(vulkandevice->device, vulkandevice->commandPool, 0);
   // offscreenbuffer->width = swapChain->swapChainExtent.width;
  //  offscreenbuffer->height = swapChain->swapChainExtent.height;
  //  offscreenbuffer->recreate();
  //  offscreenbuffer->colorFormat = swapChain->swapChainImageFormat;
    imagesInFlight.resize(swapChain->swapChainImages.size(), VK_NULL_HANDLE);
}

void VulkanBase::createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();

    extensions.push_back("VK_KHR_get_physical_device_properties2");
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &vkinstance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void VulkanBase::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkinstance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkinstance, &deviceCount, devices.data());
    /*
    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            msaaSamples = getMaxUsableSampleCount();
            break;
        }
    }
    */
   
    vkPhysicalDevice = devices[0];
    VkPhysicalDeviceProperties p;
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &p);
    if (vkPhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}


void VulkanBase::setupSurface() {
    
    if (glfwCreateWindowSurface(vkinstance, window, nullptr, &vksurface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    
}

void VulkanBase::createCommandBuffers() {
    commandBuffers.resize(swapChain->swapChainFramebuffers.size());
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vulkandevice->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();
    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}
void VulkanBase::beginCommandBuffers(unsigned int index){
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[index], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}
bool VulkanBase::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void VulkanBase::createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapChain->swapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(vulkandevice->device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(vulkandevice->device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(vulkandevice->device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}
uint32_t VulkanBase::getFrameIndex()
{
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain->swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    /*if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }*/
    submitCommandBuffers.push_back(commandBuffers[imageIndex]);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }
    this->imageIndex = imageIndex;
    return imageIndex;
}

void VulkanBase::presentQueue(VkResult &result)
{
    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size());;
    submitInfo.pCommandBuffers =submitCommandBuffers.data();

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    if (vkQueueSubmit(queue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChain->swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

     result = vkQueuePresentKHR(queue, &presentInfo);

     currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
     submitCommandBuffers.clear();
}

void VulkanBase::setupDebugMessenger() {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(vkinstance, &createInfo, nullptr, &vkDebugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

std::vector<const char*> VulkanBase::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void VulkanBase::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

VkResult VulkanBase::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanBase::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}



VulkanBase::VulkanBase()
{
}

VulkanBase::~VulkanBase()
{
  //  delete offscreenbuffer;
    delete swapChain;


    for (int i = 0; i < commandBuffers.size(); i++) {
        vkFreeCommandBuffers(device, vulkandevice->commandPool, 1, &commandBuffers[i]);
   }
    

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }


    delete vulkandevice;
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(vkinstance, vkDebugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(vkinstance, vksurface, nullptr);
    vkDestroyInstance(vkinstance, nullptr);
    
    
    
    
   // delete objectPipeline;
}
