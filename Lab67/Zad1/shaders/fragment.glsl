#version 330 core

in vec2 fragUV;
in vec3 fragLightCoef;

uniform sampler2D textureSampler;
uniform vec3 objectColor;
uniform bool isLightSphere;

out vec4 outColor;

void main()
{
    if(isLightSphere)
    {
        outColor = vec4(fragLightCoef, 1.0);
        return;
    }
    vec4 texColor = texture(textureSampler, fragUV);

    if (texColor.a < 0.5)
        discard;

    outColor = vec4(fragLightCoef, 1.0) * texColor;
}
