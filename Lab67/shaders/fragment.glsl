#version 330 core

in vec2 fragUV;                     

uniform sampler2D textureSampler;    
uniform vec3 objectColor;           
out vec4 outColor;

void main()
{
    vec4 texColor = texture(textureSampler, fragUV);

    if(texColor.a < 0.5) discard;
    //  sama tekstura
     outColor = texColor;
    
    //  tekstura i kolor
    //outColor = texColor * vec4(objectColor, 1.0);
}
