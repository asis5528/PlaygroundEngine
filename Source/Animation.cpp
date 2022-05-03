#include "Animation.h"
//Animation::Animation(AnimationData animationData,float duration) {

	//this->animationData = animationData;
	//this->duration = duration;
//}

void Animation::boneTransform(float time)
{
	processedMatrices.clear();
	glm::mat4 identity(1.0);
	readAnimation(time,animationData, identity);
	float k = 2.;
}

void Animation::readAnimation(float time, AnimationData& animData, const glm::mat4& Parentmatrix)
{
	glm::mat4 globalMatrix(1.0);
	if(animData.animationTransformation.size()>0){
		float TicksPerSecond = tps;
		float TimeInTicks = time * TicksPerSecond;
		float dura = animData.animationTransformation[animData.animationTransformation.size()-1].time - animData.animationTransformation[0].time;
		float animationTime = fmod(TimeInTicks, dura);
		animationTime += animData.animationTransformation[0].time;

		glm::vec3 position;
		CalcInterpolatedPosition(position, animationTime, animData);

		glm::quat rotation;
		CalcInterpolatedRotation(rotation, animationTime, animData);
		
		glm::vec3 scale(1.0);

		glm::mat4 translationMatrix(1.0);
		glm::mat4 rotationMatrix(1.0);
		glm::mat4 scaleMatrix(1.0);

		translationMatrix = glm::translate(translationMatrix, position);
		rotationMatrix = glm::toMat4(rotation);
		scaleMatrix = glm::scale(scaleMatrix, scale);

		glm::mat4 localtransformationMatrix = translationMatrix * rotationMatrix * scaleMatrix;
		globalMatrix = Parentmatrix * localtransformationMatrix;

		BoneProcessedMat processedMatrix;
		processedMatrix.name = animData.name;
		processedMatrix.transformedBone = globalMatrix;
		processedMatrices.push_back(processedMatrix);

	}
	for (int i = 0; i < animData.childAnimationData.size(); i++) {
		readAnimation(time, animData.childAnimationData[i], globalMatrix);
	}
}

void Animation::CalcInterpolatedRotation(glm::quat& rotation, float animationTime,AnimationData& animData)
{
	float size = animData.animationTransformation.size();
	if (animData.animationTransformation.size() == 1) {
		rotation = animData.animationTransformation[0].rotation;
		return;
	}
	unsigned int RotationIndex = FindRotation(animationTime, animData);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < size);
	float DeltaTime = (float)(animData.animationTransformation[NextRotationIndex].time - animData.animationTransformation[RotationIndex].time);
	float Factor = (animationTime - (float)animData.animationTransformation[RotationIndex].time) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::quat& StartRotationQ = animData.animationTransformation[RotationIndex].rotation;
	const  glm::quat& EndRotationQ = animData.animationTransformation[NextRotationIndex].rotation;

	glm::quat q1;
	q1.w = StartRotationQ.w; q1.x = StartRotationQ.x; q1.y = StartRotationQ.y; q1.z = StartRotationQ.z;
	glm::quat q2;
	q2.w = EndRotationQ.w; q2.x = EndRotationQ.x; q2.y = EndRotationQ.y; q2.z = EndRotationQ.z;

	glm::quat fina = glm::slerp(q1, q2, Factor);
	
	fina = glm::normalize(fina);
	rotation = fina;

	assert(animData.animationTransformation.size() > 0);

}

void Animation::CalcInterpolatedPosition(glm::vec3& position, float animationTime, AnimationData& animData)
{
	float size = animData.animationTransformation.size();
	if (animData.animationTransformation.size() == 1) {
		position = animData.animationTransformation[0].position;
		return;
	}
	unsigned int PositionIndex = FindPosition(animationTime, animData);

	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < size);

	float DeltaTime = (float)(animData.animationTransformation[NextPositionIndex].time - animData.animationTransformation[PositionIndex].time);
	float Factor = (animationTime - (float)animData.animationTransformation[PositionIndex].time) / DeltaTime;

	assert(Factor >= 0.0f && Factor <= 1.0f);
	const glm::vec3 start = animData.animationTransformation[PositionIndex].position;
	const  glm::vec3 End = animData.animationTransformation[NextPositionIndex].position;
	glm::vec3 Delta = End - start;
	position = start + Factor * Delta;
}

unsigned int Animation::FindRotation(float animationTime, AnimationData& animData)
{

	assert(animData.animationTransformation.size() > 0);
	for (unsigned int i = 0; i < animData.animationTransformation.size() - 1; i++) {
		if (animationTime < (float)animData.animationTransformation[i+1].time) {

			return i;
		}
	}
	
	assert(0);

	return 0;
}

unsigned int Animation::FindPosition(float animationTime, AnimationData& animData)
{
	assert(animData.animationTransformation.size() > 0);
	for (unsigned int i = 0; i < animData.animationTransformation.size() - 1; i++) {
		if (animationTime < (float)animData.animationTransformation[i + 1].time) {

			return i;
		}
	}

	assert(0);

	return 0;
}


