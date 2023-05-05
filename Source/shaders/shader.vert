#version 450
#extension GL_EXT_multiview : enable
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view[2];
    mat4 proj[2];
} ubo;



layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;



layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 localCoord;

void main() {

vec3 v = inPosition;
//v.xyz+=(inColor.xyz*0.1)*0.6;
    gl_Position = ubo.proj[gl_ViewIndex] * ubo.view[gl_ViewIndex] * ubo.model * vec4(v, 1.0);

   // gl_Position = vec4(v, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    localCoord = v;
}