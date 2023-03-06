#version 450
#extension GL_EXT_multiview : enable
layout(binding = 0) uniform SkinnedUniformBufferObject {
    mat4 model;
    mat4 view[2];
    mat4 proj[2];
    mat4 boneMatrices[100];
} subo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in uvec4 inBoneIndices;
layout(location = 4) in vec4 inBoneWeights;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {

    vec3 v = inPosition;

    mat4 BoneTransform =subo.boneMatrices[(inBoneIndices[0])] * inBoneWeights[0];
   
    BoneTransform += subo.boneMatrices[(inBoneIndices[1])] * inBoneWeights[1];
    BoneTransform += subo.boneMatrices[(inBoneIndices[2])] * inBoneWeights[2];
   BoneTransform += subo.boneMatrices[(inBoneIndices[3])] * inBoneWeights[3];
    mat4 k = mat4(1.0);
   
    vec4 localPosition = BoneTransform* vec4(v, 1.0);
    
    //localPosition.yz *=mat2(cos(a),sin(a),-sin(a),cos(a));
    gl_Position = subo.proj[gl_ViewIndex] * subo.view[gl_ViewIndex] * subo.model * localPosition;

     if(int(inBoneIndices[0])==0){
 //   gl_Position=vec4(0.0);
    }
  //  ivec3 ind = inBoneIndices[0].xyz;

  fragColor = abs(inColor.xyz);

    fragTexCoord = inTexCoord;
}