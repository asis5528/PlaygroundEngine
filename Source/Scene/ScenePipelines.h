#pragma once
#include <vector>
#include "../Graphics/GraphicsPipeline.h"
#include "../Graphics/VulkanBase.h"
class ScenePipelines
{
public:
	std::vector<GraphicsPipeline*> pipelines;
	VulkanBase* base;
	//VkRenderPass renderPass;
	ScenePipelines(VulkanBase *base , VkRenderPass renderPass);
	void recreate(VkRenderPass renderPass, VkExtent2D extent);
	~ScenePipelines();
};

