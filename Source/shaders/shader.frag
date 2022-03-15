#version 450

layout(binding = 2) uniform sampler2D texSampler;

layout(binding = 1) uniform ubo1 {
    float time1;
    float time2;
    float time3;
} uboo;
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = mix(vec4(fragColor,1.),texture(texSampler, fragTexCoord),0.99)*(sin(uboo.time1*5.));
   // outColor = vec4(1.,0.,0.,1.);
    
}