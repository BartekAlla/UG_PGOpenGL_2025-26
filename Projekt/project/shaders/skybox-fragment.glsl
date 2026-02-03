#version 330 core
in vec3 TexCoord;

out vec4 outColor;

uniform samplerCube tex_skybox;

void main()
{
    outColor = texture(tex_skybox, TexCoord);
}