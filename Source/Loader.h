#pragma once
#include "Scene/Scene.h"
class Loader
{
public:
	VulkanBase* base;
	Scene* scene;
	 Loader(VulkanBase* base, Scene* scene);
	 void load(int path_count, const char* paths[]);

};

