#pragma once
#include "../VulkanBase.h"
#include "../../Imgui/imgui.h"
#include "../../Imgui/imgui_impl_glfw.h"
#include "../../Imgui/imgui_impl_vulkan.h"

class VulkanImgui
{
public:
	VulkanBase* base;
	ImGui_ImplVulkanH_Window g_MainWindowData;
	GLFWwindow* window;
	VkPipelineCache pipelineCache;
	std::vector<VkFramebuffer> imguiFramebuffers;
	VkRenderPass imGuiRenderPass;
	VkDescriptorPool imguiDescriptorPool;
	std::vector<VkCommandBuffer> imguiCommandBuffer;
	VkAllocationCallbacks* allocator = NULL;
	VulkanImgui(VulkanBase* base, GLFWwindow* window);
	~VulkanImgui();
	void recreate();
	void init();
	void draw(uint32_t imageIndex);
private:
	void createImguiRenderPass();
	void createFramebuffers();
	void createCommandBuffers();
	void createDescriptorPool();
	
	

};

