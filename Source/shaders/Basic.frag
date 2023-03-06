#version 450
#extension GL_EXT_multiview : enable


layout(binding = 1) uniform BasicUbo {
   vec4 color;
} ubo;
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 localCoord;
layout(location = 0) out vec4 outColor;
float hash(vec3 p)  // replace this by something better
{
    p  = fract( p*0.3183099+.1 );
	p *= 17.0;
    return fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
}

float noise( in vec3 x )
{
    vec3 i = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
	
    return mix(mix(mix( hash(i+vec3(0,0,0)), 
                        hash(i+vec3(1,0,0)),f.x),
                   mix( hash(i+vec3(0,1,0)), 
                        hash(i+vec3(1,1,0)),f.x),f.y),
               mix(mix( hash(i+vec3(0,0,1)), 
                        hash(i+vec3(1,0,1)),f.x),
                   mix( hash(i+vec3(0,1,1)), 
                        hash(i+vec3(1,1,1)),f.x),f.y),f.z);
}
float map(vec3 p ){
float time = ubo.color.a;
    float n = noise(p*3.+vec3(time*1.,-time*2.,0.));
        float h = -((length(p)-2.1)+n*0.25);
        return h;
}
void main() {
   // outColor = mix(vec4(fragColor,1.),texture(texSampler, fragTexCoord),0.99)*(sin(uboo.time1*5.));
   // outColor = pow(texture(texSampler, fragTexCoord),vec4(1./2.2));
    //outColor = vec4(fragColor,1.);

    outColor = vec4(vec3(float(gl_ViewIndex+1)),1.);
    outColor=ubo.color;
    outColor = vec4(localCoord,1.);
    float time = ubo.color.a;
    vec3 camPos = ubo.color.rgb;
    vec3 ro = vec3(camPos);
    vec3 rd = normalize(localCoord-camPos);

    float t = length(camPos)-2.;
    vec4 sum=vec4(0.,0.,0.,0.);
    vec3 rp;
    for(uint i= 0;i<123;i++){
        
    
        vec3 p = ro+rd*t;
        vec3 q = p;
    
    float h = map(p);
        
        vec4 col1 = vec4(h)*vec4(0.245); 
       
        if(h>0.001){
        
        
        
        vec4 c =mix(vec4(1.,0.7,0.4,1.),vec4(0.5,0.5,1.,1.),h*2.);
        c.rgb = vec3(0.5);
               float dif = smoothstep(-0.8,0.2,p.z)+0.5;
               dif = clamp(dif,0.,1.);
               c.rgb*=dif*vec3(1.,0.8,0.5)+vec3(0.5,0.7,0.9)*0.87;
                sum+=col1*c*vec4(1.,1.,1.,1.)*(1.-sum.a);
      
       
        
        }
        
        t+=0.01;
    
    
    }
   
   outColor = vec4(sum);
   float line =1.-smoothstep(2.,1.95,abs(localCoord.y));
    line += 1.-smoothstep(2.,1.95,abs(localCoord.x));
    line =clamp(line,0.,1.);
  line *= 1.-smoothstep(2.,1.95,abs(localCoord.z));
   
   //sum=mix(sum,vec4(line),line);
   sum+=line;
   outColor.rgb = sum.rgb;
   outColor.a=sum.a;
   outColor.rgb*=outColor.a;
   
}