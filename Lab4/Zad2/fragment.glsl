#version 330 core

in vec3 vNormal;
in vec3 vWorldPos;

out vec4 outColor;

uniform vec3 objectColor;      
uniform vec3 lightDir = vec3(0.3, 1.0, 0.4); 
uniform float time;  

void main()
{
    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightDir);
    float diff = max(dot(N, L), 0.0);
    float h = clamp((vWorldPos.y + 2.0) / 4.0, 0.0, 1.0);
    vec3 baseGradient = mix(objectColor * 0.3, objectColor, h);
    vec3 finalColor = baseGradient * (0.4 + diff * 0.6);

    outColor = vec4(finalColor, 1.0);
}
