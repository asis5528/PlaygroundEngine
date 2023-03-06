#version 450

layout(binding = 2) uniform sampler3D texSampler;

layout(binding = 1) uniform BasicUbo {
   vec4 color;
} ubo;
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 localCoord;
layout(location = 0) out vec4 outColor;

void main() {
   // outColor = mix(vec4(fragColor,1.),texture(texSampler, fragTexCoord),0.99)*(sin(uboo.time1*5.));
   vec3 CamPos = ubo.color.rgb;
   vec3 vpos = localCoord;
   vec3 dir = normalize(vpos-CamPos);
   float t = 0.;
    vec3 p = vpos;
    float hit = 0.;
   for(int i = 0;i<54;i++){
       if(max(abs(p.x), max(abs(p.y), abs(p.z))) < 0.5 + 0.001){
        
        p= vpos+dir*t;
            float h = texture(texSampler, p+vec3(0.5)).r;
           if(h<0.01){ hit = 1.; break;}
          t+=h;
       }
   }

   vec3 ro = localCoord;
    outColor = pow(texture(texSampler,(localCoord/vec3(4.))+0.5),vec4(1./2.2))*vec4(1.,0.7,0.7,1.);
    outColor.rgb = vec3(ubo.color.rgb);
    outColor.rgb = vec3(1.);
    if(hit>0.1){
     outColor.rgb = vpos+dir*t;
    }
 //   float stp = step(0.0,localCoord.x);

 //  outColor.rgb = mix(vec3(1.,0.,0.),vec3(0.,1.,0.),stp);
   //outColor = vec4(1.,1.,0.,1.);
}