#version 450
layout (location = 0) out vec2 texcoord;

void main() {
    int i = gl_VertexIndex;
     texcoord = vec2(mod(float(i+1)*sign(float(i)),2.0),mod(float(i),2.))*4.;
	
    gl_Position = vec4(texcoord*2.-1.,0.0,1.0);
   
}