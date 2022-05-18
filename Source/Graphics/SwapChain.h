#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "VulkanImage.h"
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class SwapChain
{

public:
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkRenderPass renderPass;
	bool framebufferResized = false;
	SwapChain(VkPhysicalDevice &physicalDevice,VkDevice &device,VkSurfaceKHR &surface,GLFWwindow* &window, VkQueue &queue, VkSampleCountFlagBits msaaSamples);
	~SwapChain();
	void cleanSwapChain();
	void create();
	void beginRenderPass(VkCommandBuffer commandBuffer,  unsigned int index);

private:
	VkSurfaceKHR surface;
	VulkanImage *framebufferColorImage;
	VulkanImage* framebufferDepthImage;
//	VkFramebuffer frameBuffer;
	VkSampleCountFlagBits msaaSamples;
	VkQueue queue;
	GLFWwindow* window;
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void createRenderPass();
	void createFrameBuffer();
	void createImageViews();



};

