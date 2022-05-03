#pragma once
#include <string>
#include "AnimationData.h"
struct BoneProcessedMat {
	std::string name;
	glm::mat4 transformedBone;
};
class Animation
{
public:
	std::string name;
	AnimationData animationData;
	float tps = 25.0f;
	float duration;
	glm::mat4  m_GlobalInverseTransform;
	
	std::vector<BoneProcessedMat> processedMatrices;
//	Animation(AnimationData animationData,float duration);
	void boneTransform(float time);
	void readAnimation(float time, AnimationData& animData, const glm::mat4& Parentmatrix);
	void CalcInterpolatedRotation(glm::quat& rotation, float animationTime, AnimationData& animData);
	void CalcInterpolatedPosition(glm::vec3& position, float animationTime, AnimationData& animData);
	unsigned int FindRotation(float animationTime, AnimationData& animData);
	unsigned int FindPosition(float animationTime, AnimationData& animData);
};

