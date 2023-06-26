#pragma once
#include "../Scene/Scene.h"
class EditorGui
{
public:
	Scene* m_scene;
	VulkanBase* m_base;
	int frameID = 0;
	std::vector<VkDescriptorSet> ds;
	VkSampler texturesamp;
	EditorGui(Scene *scene, VulkanBase* base);
	void createdescriptor(VulkanTexture& tex);
	void run();

	void tree(Node& node);
	void setupGui();
	void setupStyle();
};

