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
   // outColor = mix(vec4(fragColor,1.),texture(texSampler, fragTexCoord),0.99)*(sin(uboo.time1*5.));
    outColor = pow(texture(texSampler, fragTexCoord),vec4(1./2.2))*vec4(1.,0.7,0.7,1.);
    vec3 normals = normalize(fragColor);

    vec3 sunDire = vec3(0.5,0.6,0.3);

    float diffuse =clamp(dot(sunDire,normals),0.,1.)*2.;
    outColor.rgb=diffuse*outColor.rgb+vec3(0.4,0.7,1.)*0.2;
  //outColor.rgb = fragColor;
   
    //outColor = vec4(fragColor,1.);
    float n = texture(texSampler, 1.-fragTexCoord).z*0.8;
 //   n = clamp(n,0.,1.);
    outColor = n*vec4(1.2,0.8,1.2,1.);
   // outColor.rgb = pow(outColor.rgb,vec3(1.7));
    outColor.rgb+=0.2;
    outColor.a = 1.;
   // outColor.rgb+=0.2;

    outColor.rgb*=outColor.a;
     float k = texture(texSampler, fragTexCoord).w;
    outColor = texture(texSampler, fragTexCoord);
   // outColor = mix(vec4(0.2,0.1,3.,1.),vec4(1.,0.5,0.1,1.),1.-k)*pow(k,0.6)*3.;
 //   outColor = vec4(k);
  //  outColor+=vec4(0.2);
  //  outColor.a = 1.;
  //   outColor.rgb *= outColor.a;
}