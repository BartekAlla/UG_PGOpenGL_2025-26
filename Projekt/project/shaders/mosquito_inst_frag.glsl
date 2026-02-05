#version 330 core
in vec2 fragUV;
out vec4 outColor;

uniform sampler2D textureSampler;

void main()
{
    vec4 c = texture(textureSampler, fragUV);
    if(c.a < 0.2) discard;
    outColor = c;
}
