#version 450


layout (location = 0) in vec2 texcoord;
layout(location = 0) out vec4 outColor;
layout(binding = 0) uniform sampler2DArray texSampler;
//layout(binding = 1) uniform sampler2D texSampler;


layout (constant_id = 0) const int NUM_SAMPLES = 8;


// Manual resolve for MSAA samples 
vec4 resolve(sampler2DMS tex, ivec2 uv)
{
	vec4 result = vec4(0.0);	   
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		vec4 val = texelFetch(tex, uv, i); 
		result += val;
	}    
	// Average resolved samples
	return result / float(NUM_SAMPLES);
}

void main() {
vec2 uv = texcoord;

//ivec2 attDim = textureSize(texSampler);
	//ivec2 UV = ivec2(uv * attDim);

float c = smoothstep(0.25,0.1,length(uv-0.5));

//uv.y+=sin(uv.x*105.)*0.01;
    //outColor = mix(texture(texSampler,uv,0),vec4(1.-c),0.)*2.;
	outColor = texture(texSampler,vec3(uv,0.2));
	//outColor = vec4(1.,0.,0.,1.);
	//outColor = vec4(1.,0.,0.,1.);
	//outColor = resolve(texSampler,UV);
}