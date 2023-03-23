#pragma once
#include "Graphics/VulkanBase.h"
#include "Object.h"
#include "quad.h"
#include "SceneObjects.h"
#include "Material.h"
class Scene
{
	
public:
	std::vector<GraphicsPipeline*> pipelines;
	std::vector<VulkanTexture> textures;
	std::vector<Material> materials;
	std::vector<Mesh> meshes;
	std::vector<Animation> animations;
	std::vector<Object> objects;
	std::vector<Quad*> quads;
	glm::vec2 viewport_resolution;
	Node rootNode;
	std::vector<Framebuffer*> framebuffers;
	VkDescriptorPool descriptorPool;
	VulkanTexture computeTexture;
	VulkanBase* base;
	glm::mat4 proj[2];
	glm::mat4 view[2];
	float offset = 2.;


	Scene(VulkanBase *vbase);
	void init();
	void update();
	void renderPass();
	void createSceneDescriptor();
	void cleanUniforms();
	void cleanDescriptors();
	void cleanTextures();
	void cleanMeshes();
	void cleanQuads();
	void clean();
	void recreate();


};

