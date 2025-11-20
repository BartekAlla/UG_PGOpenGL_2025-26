#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

out vec3 vNormal;
out vec3 vWorldPos;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

void main()
{
    vNormal   = mat3(matModel) * inNormal;
    vWorldPos = vec3(matModel * vec4(inPosition, 1.0));

    gl_Position = matProj * matView * matModel * vec4(inPosition, 1.0);
}
