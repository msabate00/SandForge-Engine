#version 330 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec4 aCol; // normalizado desde UNORM8
out vec4 vCol;
uniform vec2 uView; // (w,h) en px
void main(){
	vCol = aCol;
	vec2 p = aPos / uView * 2.0 - 1.0;
	gl_Position = vec4(p.x, -p.y, 0.0, 1.0);
}