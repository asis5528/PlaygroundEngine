#version 450


layout (location = 0) in vec2 texcoord;
layout(location = 0) out vec4 outColor;
layout(binding = 0) uniform sampler2DArray texSampler;

layout(binding = 1) uniform sampler2DArray depthSampler;


void main() {
	vec2 uv = texcoord;



	outColor = texture(texSampler,vec3(uv,0));
	outColor.a = 1.;
	outColor.rgb*=outColor.a;
	//optimized
//	outColor = vec4(pow(texelFetch(depthSampler,ivec3(gl_FragCoord.xy,0),0).rrr,vec3(12.2)),1.); 
	
}