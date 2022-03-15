#pragma once
#include "Graphics/GraphicsPipeline.h"
#include "Graphics/VulkanDescriptor.h"
#include "Shader.h"
class Object
{
public:
	Shader shader;
	uint32_t meshID;
	uint32_t textureID;
	glm::mat4 ModelMatrix;
	

};

