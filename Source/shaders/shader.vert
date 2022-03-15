#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {

vec3 v = inPosition;
//v.xyz+=(inColor.xyz*0.1)*0.6;
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(v, 1.0);
   // gl_Position = vec4(v, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}