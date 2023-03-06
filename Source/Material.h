#pragma once
#include "Graphics/GraphicsPipeline.h"

enum MaterialType {
	BasicMaterial, 
	TexturedMaterial,
	PBR 
	
};
struct MaterialBasicData {
	glm::vec4 DiffuseColor;
	float time;
};
struct MaterialTexturedData : public MaterialBasicData {
	int DiffuseTexture;
};
struct MaterialPBRData : public MaterialBasicData {
	int DiffuseTexture = -1;
};

class Material
{
public:
	void* materialData;
	GraphicsPipeline *pipeline;
	GraphicsPipeline *skinnedPipeline;
	MaterialType type;
};

