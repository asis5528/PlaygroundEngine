#pragma once
#include "Graphics/GraphicsPipeline.h"
#include "Graphics/VulkanDescriptor.h"
#include "Shader.h"
#include "Animation.h"

class Object
{
public:
	Shader shader;
	std::vector<uint32_t> meshID;
	uint32_t textureID;
	glm::mat4 ModelMatrix;
	glm::mat4 finalMatrix;
	bool hasMesh;
	std::string name;
	//std::vector<Animation> animations;
	unsigned int animationID;
	bool hasAnimation;
	std::vector<Object> child;
	void update();
};
struct Node{
	uint32_t ObjectID;
	std::vector<Node> children;

	bool traverse(Node &node,int index,int count = 0) {
		return false;

	}

	void updateTransoformation( const glm::mat4& Parentmatrix, std::vector<Object> &objects) {
		for (int i = 0; i < this->children.size(); i++) {
			//  std::cout << scene->objects[node.children[i].ObjectID].name << "\n";
			objects[this->children[i].ObjectID].finalMatrix = Parentmatrix * objects[this->children[i].ObjectID].ModelMatrix;
			this->children[i].updateTransoformation( objects[this->children[i].ObjectID].finalMatrix,objects);
		}
	}
};

