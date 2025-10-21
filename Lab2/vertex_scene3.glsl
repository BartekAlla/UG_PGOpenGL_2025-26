#version 330 core

layout(location = 0) in vec2 aPos;

uniform vec2 uOffset;   

out vec2 vPos;         

void main()
{
    vec2 rotated = vec2(aPos.y, -aPos.x);
    vec2 pos = (rotated + uOffset);
    gl_Position = vec4(pos, 0.0, 1.0);
    vPos = rotated;
}