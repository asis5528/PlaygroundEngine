#pragma once
#include "Graphics/Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <map>
#include "AnimationData.h"
#include "Animation.h"
#include "Object.h"
#include "Scene.h"
class Model
{
public:
	std::vector<Mesh> meshes;
	std::vector < glm::mat4> transformations;
	std::vector<Animation> animations;
	std::vector<Texture> textures;
	std::vector<Material> materials;
	const Scene* engineScene;
	const aiScene* m_pScene;
	std::vector<Object> objects;
	Object rootObj;
	Node rootNode;
	unsigned int scObjectSize;
	Model(const char* path,const Scene *enscene);
	//void processNode(aiNode* node, const aiScene* scene);
	void processNode(aiNode* node, const aiScene* scene, Node &nodechild);
	void processMesh(aiMesh* mesh, const aiScene* scene, Mesh& meshbuffer);
	inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from);
	void readAnimation(const aiNode* pNode, AnimationData* aData);
	Texture loadTexture(const aiTexture* tex);
	//void readAnimation(const aiNode* pNode);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	std::map<std::string, int> m_BoneMapping;
	glm::mat4 globalInverseTransform;
	int m_NumBones = 0;
	std::vector<BoneInfo> m_BoneInfo;
};

