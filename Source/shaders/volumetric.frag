#version 450

layout(binding = 2) uniform sampler3D texSampler;

layout(binding = 1) uniform BasicUbo {
   vec4 color;
   vec4 sub;
} ubo;
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 localCoord;
layout(location=3) in vec3 cameraPos;
layout(location = 0) out vec4 outColor;


vec2 map (vec3 p){
float k = 1./64.;
vec3 cpos =  p+vec3(0.5);
vec3 ppos = cpos;
vec4 h1 = texture(texSampler,ppos );
//vec3 h1 = texelFetch(texSampler,ivec3(cpos.x*64.-1.,cpos.y*64.,cpos.z*64.),0).rgb;
float time = ubo.color.a;

float h = h1.r;
vec3 q = p;
float y = sin(time*0.02);
float off = sin((1.57075-abs(y-0.1)));
q.y+=y*0.3;
q.x+=(sin(time)*0.3)*off;

q.z+=(cos(time)*0.3)*off;

float mat = 0.;
vec3 pq = p;
pq+=h1.gba;
float eraser  = length(pq)-(0.02);

float d =  h1.r;
if(eraser<d&&time>0.){
mat = 1.5;
d = eraser;
}
return vec2(d,mat);
}

vec3 calcNormal( in vec3 pos )
{
 vec3 eps = vec3(0.002,0.0,0.0);
	return normalize( vec3(
           map(pos+eps.xyy).x - map(pos-eps.xyy).x,
           map(pos+eps.yxy).x - map(pos-eps.yxy).x,
           map(pos+eps.yyx).x - map(pos-eps.yyx).x ) );
           }

vec4 rndC(vec3 u) {
    vec3 R = vec3(128.,128.,128.),
         U = u*R + .5,
         F = fract(U);
    U = floor(U) + F*F*(3.-2.*F); 
 // U = floor(U) + F*F*F*(F*(F*6.-15.)+10.);   // use if you want smooth gradients
    return texture( texSampler, u );
}
vec3 norm(vec3 u){
    float ep = 0.000001;
    return normalize(vec3(
    rndC(vec3(u.x+ep,u.y,u.z)).w-rndC(vec3(u.x-ep,u.y,u.z)).w,
    rndC(vec3(u.x,u.y+ep,u.z)).w-rndC(vec3(u.x,u.y-ep,u.z)).w,
    rndC(vec3(u.x,u.y,u.z+ep)).w-rndC(vec3(u.x,u.y,u.z-ep)).w
    ));

}

const vec3 sundir = vec3(-0.7071,1.0,1.7071);

void main() {
   // outColor = mix(vec4(fragColor,1.),texture(texSampler, fragTexCoord),0.99)*(sin(uboo.time1*5.));
   vec3 CamPos = cameraPos;
   vec3 vpos = localCoord;
   vec3 dir = normalize(localCoord-cameraPos);
   float t = 0.;
    vec3 p = vpos;
    float hit = 0.;
    float mat = 0.;
    /*
   for(int i = 0;i<254;i++){
  

       if(max(abs(p.x), max(abs(p.y), abs(p.z))) < 0.50001){
        
        p= vpos+dir*t;
        
            vec2 h = map(p);
           if(h.x<0.0001){ hit = 1.; mat = h.y; break;}
          t+=h.x;
       }
   }*/

   vec4 col = vec4(0.);
    for(int i = 0;i<128;i++){
  

       if(max(abs(p.x), max(abs(p.y), abs(p.z))) < 0.50001){
        
        p= vpos+dir*t;
        vec3 pos = p+0.5;
            float den = rndC(pos ).w;
            if(den>0.0){

                float sunDiffuse = clamp((den - rndC(pos+1.*sundir).w)/1., 0.0, 1.0 ); 

            den*=(0.35);   
		vec3  finalShade = vec3(1.,0.5,0.1)*sunDiffuse+vec3(0.8,0.8,1.0)*0.1; 
	
        vec4  c = vec4( mix( vec3(0.8,0.75,0.65), vec3(0.25,0.3,0.85), den*0.01 ), den ); 
        
       
        c.a*=1.;
   //c.xyz = vec3(1.);
	c.xyz = c.xyz*0.5+finalShade*0.7;
		//c.xyz = vec3(1.)*(max(dot(norm(pos),vec3(0.3,1.0,-1.0)),0.0)+den);



     //   c.w *= 0.4; 
     c.rgb *= c.a; 
		col += c*(1.0-col.a); 
      
            }

            t+=0.0078;
            }
       }
       col.rgb*=1.+col.a*0.35;
       
       //co.rgb = mix(vec3(1.,0.7,0.7),col.rgb,0.7);
      // col.rgb = pow(col.rgb,vec3())
    //   col *= vec4(abs(p),1.);
   vec3 ro = localCoord;


    outColor.rgb = vec3(CamPos);
    outColor.a = 0.;
    if(hit>0.1){
      vec3 n=normalize(calcNormal( vpos+dir*t));
      vec3 light = vec3(10.,10.,10.);

      float dif = clamp(dot(normalize(light),n),0.,1.);

      vec3 reflectDir = reflect(normalize(light), n);
      float spec = pow(max(dot(dir, reflectDir), 0.0), 32.);

      outColor.rgb = vec3(0.8,0.5,0.2)*dif*0.7+0.1+clamp(dot(dir,n)+1.,0.,1.)*0.5*vec3(1.,1.,1.)+spec*0.5;
      if(mat>1.){
      outColor.rgb = vec3(0.8,0.0,0.0)*dif*0.5+0.5+clamp(dot(dir,n)+1.,0.,1.)*0.3* vec3(0.8,0.0,0.0);
      }
      outColor.rgb = pow(outColor.rgb,vec3(1./1.5));
     outColor.a = 1.;
    }
    outColor.rgb = col.rgb;
    outColor.a = 1.0;
   outColor.rgb*=outColor.a;

}