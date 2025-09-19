#version 330 core
in vec2 vUV; in vec4 vColor; out vec4 oC;
uniform sampler2D uTex;
void main(){ oC = texture(uTex, vUV) * vColor; }