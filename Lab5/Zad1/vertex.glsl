#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;   

out vec2 fragUV;                     

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

void main()
{
    fragUV = inUV;                   
    gl_Position = matProj * matView * matModel * vec4(inPos, 1.0);
}
