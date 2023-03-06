#pragma once
#include "../Scene.h"
class EditorGui
{
public:
	Scene* m_scene;
	VulkanBase* m_base;
	int frameID = 0;
	EditorGui(Scene *scene, VulkanBase* base);
	void run();
	void tree(Node& node);
	void setupGui();
	void setupStyle();
};

