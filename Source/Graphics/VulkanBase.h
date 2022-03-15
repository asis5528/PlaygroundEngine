#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "VulkanDevice.h"
#include "SwapChain.h"
#include "Framebuffer.h"
#include "GraphicsPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptor.h"
class VulkanBase {

public:
	VkInstance vkinstance;
	VkSurfaceKHR vksurface;
	VkPhysicalDevice vkPhysicalDevice;
	VkDevice device;
	VkQueue queue;
	VkDebugUtilsMessengerEXT vkDebugMessenger;
	VkSampleCountFlagBits msaaSamples;
	VulkanDevice* vulkandevice;
	SwapChain* swapChain;
	Framebuffer* offscreenbuffer;
	VkPipelineCache pipelineCache;
//	GraphicsPipeline* objectPipeline;
	//GraphicsPipeline* quadPipeline;
	VBuffer* vbuffer;
	VulkanDescriptor* vulkandescriptor;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	std::vector<VkCommandBuffer>  commandBuffers;
	size_t currentFrame = 0;
	uint32_t imageIndex;
	std::vector<VkCommandBuffer> submitCommandBuffers;
	void init(GLFWwindow* window);
	void setupSurface();
	void beginCommandBuffers(unsigned int index);
	uint32_t getFrameIndex();
	void presentQueue(VkResult &result);
	void recreate();
	VulkanBase();
	~VulkanBase();

private:
	GLFWwindow* window;
	const int MAX_FRAMES_IN_FLIGHT = 2;
	
	void createInstance();
	void pickPhysicalDevice();
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	void createCommandBuffers();
	void createSyncObjects();
};