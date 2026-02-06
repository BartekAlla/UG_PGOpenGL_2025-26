#version 330 core

in vec2 fragUV;
in vec3 fragLightCoef;

uniform sampler2D textureSampler;
uniform vec3 objectColor;
uniform bool isLightSphere;
uniform bool  enableFog;
uniform vec3  fogColor;
uniform float fogDensity;    
uniform float fogStart;      
uniform float fogEnd;         
uniform int   fogMode;        

in vec3 fragPos;
uniform vec3 cameraPos;

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
    if (enableFog)
    {
        float dist = length(cameraPos - fragPos);

        float fogFactor = 1.0;
        if (fogMode == 0) // linear
        {
            fogFactor = (fogEnd - dist) / (fogEnd - fogStart);
        }
        else if (fogMode == 1) // exp
        {
            fogFactor = exp(-fogDensity * dist);
        }
        else // exp2
        {
            float d = fogDensity * dist;
            fogFactor = exp(-(d * d));
        }

        fogFactor = clamp(fogFactor, 0.0, 1.0);

        outColor.rgb = mix(fogColor, outColor.rgb, fogFactor);
    }
}
