#version 450

layout(binding = 2) uniform sampler3D texSampler;

layout(binding = 1) uniform BasicUbo {
   vec4 color;
} ubo;
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 localCoord;
layout(location = 0) out vec4 outColor;


vec2 map (vec3 p){

vec3 h1 = texture(texSampler, p+vec3(0.5)).rgb;
float time = h1.g*2.;

float h = h1.r;
vec3 q = p;
float y = sin(time*0.02);
float off = sin((1.57075-abs(y-0.1)));
q.y+=y*0.3;
q.x+=(sin(time)*0.3)*off;

q.z+=(cos(time)*0.3)*off;

float mat = 0.;
float eraser  = length(q)-(0.02+time*0.0002);

float d =  h1.r;
if(eraser<d&&time<160.){
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



void main() {
   // outColor = mix(vec4(fragColor,1.),texture(texSampler, fragTexCoord),0.99)*(sin(uboo.time1*5.));
   vec3 CamPos = ubo.color.rgb;
   vec3 vpos = localCoord;
   vec3 dir = normalize(vpos-CamPos);
   float t = 0.;
    vec3 p = vpos;
    float hit = 0.;
    float mat = 0.;
   for(int i = 0;i<54;i++){
  
 //  p = clamp(p,-0.1,0.1);
       if(max(abs(p.x), max(abs(p.y), abs(p.z))) < 0.50001){
        
        p= vpos+dir*t;
        
            vec2 h = map(p);
           if(h.x<0.01){ hit = 1.; mat = h.y; break;}
          t+=h.x;
       }
   }

   vec3 ro = localCoord;
  //  outColor = pow(texture(texSampler,(localCoord/vec3(4.))+0.5),vec4(1./2.2))*vec4(1.,0.7,0.7,1.);
   // outColor.rgb = vec3(ubo.color.rgb);
  // float sdi = length(localCoord.zy-vec2(0.0,0.))*2.;
   //sdi = min(sdi,1.);

  //  outColor.rgb = vec3(mix(0.);
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
  //   outColor.rgb=vec3(hit);
  //  outColor.rgb = vec3(texture(texSampler, vec3(localCoord.xy-0.5,1.)).r);
    outColor.rgb*=outColor.a;
 //   float stp = step(0.0,localCoord.x);

 //  outColor.rgb = mix(vec3(1.,0.,0.),vec3(0.,1.,0.),stp);
   //outColor = vec4(1.,1.,0.,1.);
}