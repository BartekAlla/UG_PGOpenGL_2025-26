#version 330 core
layout(location = 0) in vec3 inPos;

out vec3 TexCoord;

uniform mat4 matProj;
uniform mat4 matView;   
uniform float scale;

void main()
{
    TexCoord = inPos;
    vec4 pos = matProj * matView * vec4(inPos * scale, 1.0);
    gl_Position = pos.xyww; 
}